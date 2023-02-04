/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*  Etriphany
 *  ==========
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */


#include <fluent-bit/flb_info.h>
#include <fluent-bit/flb_input.h>
#include <fluent-bit/flb_input_plugin.h>
#include <fluent-bit/flb_pack.h>

#include "load.h"


static int in_load_collect(struct flb_input_instance *i_ins,
                           struct flb_config *config, void *in_context);

static void parse_avgs(char* str, struct flb_in_load_data *ld_data)
{
    char* avg;
    uint8_t i = 0;

    avg = strtok(str, " ");
    while(avg != NULL && ++i < 4) {
        switch(i) {
            case 1: ld_data->load_1 = atof(avg); break;
            case 2: ld_data->load_5 = atof(avg); break;
            case 3: ld_data->load_15 = atof(avg); break;
        }
        avg = strtok(NULL, " ");
    }
}

static int load_calc(char* proc_path, struct flb_in_load_data *ld_data)
{
    static FILE* fd;
    char buf[256];
    size_t len;

    snprintf(buf, sizeof(buf), "%s/%s", proc_path, "loadavg");
    fd = fopen(buf, "r");
    if (fd != NULL) {
        len = fread(buf, 1, sizeof(buf) - 1, fd);
        fclose(fd);

        if(len > 0) {
            parse_avgs(buf, ld_data);
            return 0;
        }
    }
    return -1;
}


static int in_load_init(struct flb_input_instance *in,
                        struct flb_config *config, void *data)
{
    int ret;
    struct flb_in_load_config *ctx;
    const char *pval = NULL;

    /* Initialize context */
    ctx = flb_malloc(sizeof(struct flb_in_load_config));
    if (!ctx) {
        return -1;
    }
    ctx->ins = in;

    /* loadavg file setting */
    pval = flb_input_get_property("proc_path", in);
    if (pval != NULL) {
        ctx->proc_path = flb_strdup(pval);
    } else {
       ctx->proc_path = DEFAULT_PROC_PATH;
    }

    /* Collection time setting */
    pval = flb_input_get_property("interval_sec", in);
    if (pval != NULL && atoi(pval) > 0) {
        ctx->interval_sec = atoi(pval);
    }
    else {
        ctx->interval_sec = DEFAULT_INTERVAL_SEC;
    }

    /* Set the context */
    flb_input_set_context(in, ctx);

    /* Set the collector */
    ret = flb_input_set_collector_time(in, in_load_collect,
                                       ctx->interval_sec,
                                       0, config);
    if (ret == -1) {
        flb_plg_error(ctx->ins, "could not set collector for load input plugin");
    }

    return 0;
}

static int in_load_collect(struct flb_input_instance *i_ins,
                           struct flb_config *config, void *in_context)
{
    int ret;
    int entries = 3; /* avg 1, 5 and 15 */
    struct flb_in_load_config *ctx = in_context;
    struct flb_in_load_data data;
    msgpack_packer mp_pck;
    msgpack_sbuffer mp_sbuf;

    ret = load_calc(ctx->proc_path, &data);
    if (ret == -1) {
        return -1;
    }

    /* Initialize local msgpack buffer */
    msgpack_sbuffer_init(&mp_sbuf);
    msgpack_packer_init(&mp_pck, &mp_sbuf, msgpack_sbuffer_write);

    /* Pack the data */
    msgpack_pack_array(&mp_pck, 2);
    flb_pack_time_now(&mp_pck);
    msgpack_pack_map(&mp_pck, entries);

    msgpack_pack_str(&mp_pck, 6);
    msgpack_pack_str_body(&mp_pck, "load.1", 6);
    msgpack_pack_float(&mp_pck, data.load_1);

    msgpack_pack_str(&mp_pck, 6);
    msgpack_pack_str_body(&mp_pck, "load.5", 6);
    msgpack_pack_float(&mp_pck, data.load_5);

    msgpack_pack_str(&mp_pck, 7);
    msgpack_pack_str_body(&mp_pck, "load.15", 7);
    msgpack_pack_float(&mp_pck, data.load_15);

    flb_input_chunk_append_raw(i_ins, NULL, 0, mp_sbuf.data, mp_sbuf.size);
    msgpack_sbuffer_destroy(&mp_sbuf);

    return 0;
}

static int in_load_exit(void *data, struct flb_config *config)
{
    struct flb_in_load_config *ctx = data;

    if (!ctx) {
        return 0;
    }

    /* done */
    flb_free(ctx);

    return 0;
}

struct flb_input_plugin in_load_plugin = {
    .name         = "load",
    .description  = "System load info",
    .cb_init      = in_load_init,
    .cb_pre_run   = NULL,
    .cb_collect   = in_load_collect,
    .cb_flush_buf = NULL,
    .cb_exit      = in_load_exit
};