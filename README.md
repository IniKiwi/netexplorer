# IniKiwi's netexplorer
icence: gpl-3.0
## use   
`inikiwi-netexplorer [-r <requests>] [-j <threads>] [-t <timeout>] [--hide-fail] [--hide-access-denied] [--hide-skipped] [--raw-output <file>] [--protocol-map <file>] <task>`
### tasks
- `explore [<protocol>://]{<ip>, hostname}[:<port>]` scan ports of a given ip or hostname. for each ports prints the requests status: `FAIL` or `OK`. explore task is multithreaded.
- `explore-file-raw <files>` immport raw task list from file and run `explore` task. example: `explore-file-raw "raw1.txt;raw2.txt"`

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
- `protocol` the protocol for scan.
    - `tcp` check the tcp connection.
    - `http` open a tcp connection and do a http request without ssl/tls.
    - `https` open a tcp connection and do a https with ssl/tls.
    - `ftp` open a tcp connection and try login as `anonymous` and list files in root directory.
    - `auto` use protocol map to assign ports to protocol.
    - `skip` skip request.
- `-r <requests>` maximum number of requests to do in a task. default is `0` (infinite).
- `-j <threads>` number of threads used for explore tasks. a thread can do one request at a time.
- `-t <timeout>` set timeout for requests. if the timeout is reached the requests is set to `FAIL`.
- `--hide-fail` hides failed requests.
- `--hide-access-denied` hides requests where ressource access is denied.
- `--hide-skipped` hides skipped requests (with protocol `skip`).
- `--raw-output` write results with `OK` status in a text file as a raw task list, compatible with `explore` task.
- `--protocol-map` set the protocol map file. default is `protocolmap.txt` (in your current shell directory).

## build
### debian based 
```bash
sudo apt-get install build-essential cmake libssl-dev
mkdir build && cd build
cmake ..
make
./inikiwi-netexplorer
```
