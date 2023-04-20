# IniKiwi's netexplorer
icence: gpl-3.0
## use   
`inikiwi-netexplorer [-r <requests>] [-j <threads>] [-t <timeout>] [--hide-fail] <task>`
### tasks
- `explore <ip>[:port]` scan ports of a given ip. for each ports prints the requests status: `FAIL` or `OK`. explore task is multithreaded.

### arguments
- `port` (optional, default is `*`) can be 
    - a number between 0 and 65535.
    - a range like `*` -> `1-65535`
    - `rand` to generate random port.
- `ip` use `[0-255]-.[0-255].[0-255].[0-255]` format or `rand` to generate random ips. examples: 
    - `192.168.0.*` for local ips.
    - `rand` -> `rand.rand.rand.rand` for full random ips.
    - `192.168.0-10.*` ranges.
    - `*.*.*.*` -> `0-255.0-255.0-255.0-255` ranges.
- `-r <requests>` maximum number of requests to do in a task. default is `0` (infinite).
- `-j <threads>` number of threads used for explore tasks. a thread can do one request at a time.
- `-t <timeout>` set timeout for requests. if the timeout is reached the requests is set to `FAIL`.
- `--hide-fail` hides failed requests (timeout). show only requests with `OK` and `SKIPPED` status.

## build
### debian based 
```bash
sudo apt-get install build-essential cmake libpthread-stubs0-dev libsqlite3-dev
mkdir build && cd build
cmake ..
make
./inikiwi-netexplorer
```