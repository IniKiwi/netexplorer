# IniKiwi's netexplorer
icence: gpl-3.0
## use   
`inikiwi-netexplorer [-r <requests>] [-j <threads>] [-t <timeout>] [--hide-fail] [--raw-output <file>] <task>`
### tasks
- `explore [<protocol>://]{<ip>, hostname}[:<port>]` scan ports of a given ip or hostname. for each ports prints the requests status: `FAIL` or `OK`. explore task is multithreaded.

### arguments
- `port` (optional, default is `*`) can be 
    - a number between 0 and 65535.
    - a range like `*` -> `1-65535`
    - `rand` to generate random port.
- `ip` use `[0-255].[0-255].[0-255].[0-255]` format or `rand` to generate random ips. examples: 
    - `192.168.0.*` for local ips.
    - `rand` -> `rand.rand.rand.rand` for full random ips.
    - `192.168.0-10.*` ranges.
    - `*.*.*.*` -> `0-255.0-255.0-255.0-255` ranges.
- `hostname` just a normal hostname like `www.google.fr` or `inikiwi.net`
- `protocol` the protocol for scan. examples: `tcp`, `udp` or `minecraft-java` *(not implemented)*.
- `-r <requests>` maximum number of requests to do in a task. default is `0` (infinite).
- `-j <threads>` number of threads used for explore tasks. a thread can do one request at a time.
- `-t <timeout>` set timeout for requests. if the timeout is reached the requests is set to `FAIL`.
- `--hide-fail` hides failed requests (timeout). show only requests with `OK` and `SKIPPED` status.
- `--raw-output` write results with `OK` status in a text file as a raw task list,  compatible with `explore` task.

## build
### debian based 
```bash
sudo apt-get install build-essential cmake libpthread-stubs0-dev libsqlite3-dev
mkdir build && cd build
cmake ..
make
./inikiwi-netexplorer
```
