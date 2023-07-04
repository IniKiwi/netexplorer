# IniKiwi's netexplorer
icence: gpl-3.0
## use   
`inikiwi-netexplorer [-r <requests>] [-j <threads>] [-t <timeout>] [--hide-fail] [--hide-access-denied] [--hide-skipped] [--raw-output <file>] [--protocol-map <file>] <task>`
### tasks
- `explore [<protocol>://]{<ip>, hostname}[:<port>]` scan ports of a given ip or hostname. for each ports prints the requests status: `FAIL` or `OK`. explore task is multithreaded.
- `explore-file-raw <files>` import raw task list from file and run `explore` task. example: `explore-file-raw "raw1.txt;raw2.txt"`
- `script <file>` run the lua script

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

# Lua scripting API

## Logger
```lua
local mylogger = Logger.new(use_stdout, logfile)
```
### methods
| function | arguments | action |
|---|---|---|
| `set_hide_fail(v)` | `v:bool` bollean value.| hide requests whit `FAIL` status. |
| `set_hide_skipped(v)` | `v:bool` bollean value.| hide requests whit `SKIPPED` status. |
| `set_hide_access_denied(v)` | `v:bool` bollean value.| hide requests whit `ACCESS DENIED` status. |
| `log(msg)` | `msg:string` string to display and log | print message to stdout (if activated) and save it in the log file (if selected) |

## ProtocolMap
```lua
local myprotocolmap = ProtocolMap.new()
myprotocolmap:import("./protocolmap.txt")
```
### methods
| function | arguments | action |
|---|---|---|
| `import(file)` | `file:string` file path.| import protocolmap from file |
| `clear()` | | clear the protocol map |
| `set_hide_access_denied(v)` | `v` bollean value.| hide requests whit `ACCESS DENIED` status. |
| `set(port, protocols)` | `port:int` the port.  `protocols:string` space separated protocols | attribute protocols to a given port. |
| `get(port)` | `port:int` the port. | return protocol list (string) for the given port. |

## NetworkTask
```lua
local mynetworktask = NetworkTask.new(mylogger, myprotocolmap)
```
### methods
| function | arguments | action |
|---|---|---|
| `decode(data)` | `data:string` or `data:list:string`  | imports tasks like `explore` task |
| `decode_file(file)` | `file:string` file path.| import tasks like `explore-file-raw` task |
| `set_max_requests(num)` | `num:int` number of maximum requests to do | like `-r` option |
| `set_raw_output(file)` | `file:string` file path. | save address of tasks result in a file that can be imported with `explore-file-raw` and `decode_file(file)` |
| `set_threads(t)` | `t:int` number of threads to use | like the `-j` option |
| `set_timeout(t)` | `t:int` timeout (us) for socket connections | like the `-t` option |
| `get_timeout()` |  | return the timeout (us) |
| `run()` |  | execute the task with all given settings |
| `register_onresult_tcp(function(addr))` | `addr:Ipv4Addr` the result address.  | set the callback when a reslut is found with the `tcp` protocol |
| `register_onresult_http(function(addr, result))` | `addr:Ipv4Addr` the result address. `result:HttpRequestResult` the http request result object.| set the callback when a reslut is found with the `http` protocol |
| `register_onresult_https(function(addr, result))` | `addr:Ipv4Addr` the result address. `result:HttpRequestResult` the http request result object.| set the callback when a reslut is found with the `https` protocol |

## HttpRequestResult

```lua
mynetworktask:register_onresult_https(function(result_address, myhttprequestresult)
    mylogger:log(myhttprequestresult:get_content())
end)
```

### methods

| function | arguments | action |
|---|---|---|
| `get_status()` |  | return the HTTP status code. |
| `get_header(name)` | `name:string` the http header name. | return the HTTP header value as a string. |
| `get_full_header()` |  | return the full http header as a string. |
| `get_version()` |  | return the HTTP version tag as a string. |
| `get_content_size()` |  | return the size of the received content as a int. |
| `get_content()` |  | return the the received content as a string. (the content can be a binary file!)|
