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
#ifndef FLB_IN_LOAD_H
#define FLB_IN_LOAD_H

#include <fluent-bit/flb_input.h>
#include <fluent-bit/flb_config.h>
#include <fluent-bit/flb_utils.h>

#define DEFAULT_INTERVAL_SEC  10
#define DEFAULT_PROC_PATH "/proc"

struct flb_in_load_data {
    float load_1;
    float load_5;
    float load_15;
};

struct flb_in_load_config {
    char*  proc_path;
    int    interval_sec;
    struct flb_input_instance *ins;
};

#endif