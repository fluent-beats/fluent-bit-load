# Description

[Fluent Bit](https://fluentbit.io) input plugin that collects Linux host load averages, just like [upitime command](https://en.wikipedia.org/wiki/Uptime).

This plugin **will only work** on hosts running Linux, because it relies on `/proc/loadavg` file from [Procfs](https://en.wikipedia.org/wiki/Procfs).

# Requirements

- Docker
- Docker image `fluent-beats/fluent-bit-plugin-dev`

# Build
```bash
./build.sh
```

# Test
```bash
./test.sh
 ```

# Design

This plugin was desined to collect load averages from any mounted Linux `loadavg` proc file.

## Configurations

This input plugin can be configured using the following parameters:

 Key                    | Description                                   | Default
------------------------|-----------------------------------------------|------------------
 interval_sec           | Interval in seconds to collect data           | 10
 proc_path              | Path to look for loadavg file                 | /proc


