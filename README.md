# ctutorial

C tutorial with protocols, real time signal analysis and control implementation, including workspace and build configuration

## Author

Jani Yli-Alho

- [https://github.com/jylialho](https://github.com/jylialho)
- [https://www.linkedin.com/in/jylialho/](https://www.linkedin.com/in/jylialho/)

## Assignment summary [TODO]
### Task 1 results [TODO]
#### Server TCP port signal frequencies, amplitudes and shapes

| port | name | frequency  | amplitude  | shape    | period   |
| ---- | ---- | ---------- | ---------- | -------- | -------- |
| 4001 | out1 | 0.5 Hz     | -5.0 - 5.0 | sine     | 2 s      |
| 4002 | out2 | 0.25 Hz    | 0 - 5      | triangle | 4 s      |
| 4003 | out3 | 0.2 - 1 Hz | 0 / 5      | binary   | 1 - 5 s  |

### Task 2 results [TODO]
#### Control protocol [TODO]

The control messages are based on following observations from the probing and analysis of the assignment test server:

| port | quantity      | object | property  | scaling  | default       | at output    |
| ---- | ------------- | ------ | --------- | -------- | ------------- | ------------ |
| 4001 | enabled       | 1      | 14        | -        | 1             |              |
| 4001 | frequency     | 1      | 255       | * 1000   | 500 mHz       | 0.5 Hz sine  |
| 4001 | amplitude     | 1      | 170       | * 1000   | 5000          | -5.0 - 5.0   |
| 4002 | enabled       | 2      | 14        | -        | 1             |              |
| 4002 | frequency     | 2      | 255       | * 1000   | 250 mHz       | 4 s triangle |
| 4002 | amplitude     | 2      | 170       | * 1000   | 5000          | 0 - 5        |
| 4003 | enabled       | 3      | 14        | -        | 1             |              |
| 4003 | min_duration  | 3      | 42        |  ms      | 1000 ms       | 1 s 0.0      |
| 4003 | max_duration  | 3      | 43        |  ms      | 5000 ms       | 5 s 5.0      |

## Background

This project is based on an anonymous project assignment and open source C tools. Some information of the original assignment was requested not to be published and has been undisclosed.

This repository represents the results of the assignment as documented development toolchain configuration, software architecture design, implemented assignment features, tests and applications.

I found the assignment well designed, original, unambiguous and fun, serving well for the purpose of evaluating the experience level of a developer, encouraging me to invest a reasonable amount of effort for the project.

## System setup

The Debian Bookworm distribution has been used in Windows Subsystem for Linux on a modern x86_64 host computer with Windows 11 operating system:
[https://wiki.debian.org/InstallingDebianOn/Microsoft/Windows/SubsystemForLinux](https://wiki.debian.org/InstallingDebianOn/Microsoft/Windows/SubsystemForLinux)

### Development toolchain

Visual Studio Code has been used on the host computer as the code editor, with the recommended typical C extensions installed.

The project workspace on the WSL Debian instance is accessed via the VSCode remote WSL feature:
[https://code.visualstudio.com/docs/remote/wsl](https://code.visualstudio.com/docs/remote/wsl)

Docker Desktop was installed on the host system with the WSL integration enabled: [https://learn.microsoft.com/en-us/windows/wsl/tutorials/wsl-containers](https://learn.microsoft.com/en-us/windows/wsl/tutorials/wsl-containers)

### Workspace configuration

In the workspace target terminal, the package lists have been updated, installed the essential build tools with apt system package manager and cloned the repository with git source control management tool. Additionally the doxygen package was installed for generating documentation from the implemented sources.

``` bash
sudo apt update
sudo apt install build-essential
sudo apt-get install doxygen
sudo apt-get install graphviz
git clone https://github.com/jylialho/ctutorial.git
```

#### Development container [TODO]
## Server container deployment

The assignment provided a server as a undisclosed docker image and with short a port definition. For safety, as dealing with an unknown source binary, the server container shall be inspected and run in a restricted environment:

``` bash
docker load undisclosed-image
docker network create --internal internal
docker run --network internal undisclosed-image
```

Inspection revealed, that the image was based on a generic Alpine Linux and a Go server application. Some undisclosed clear text symbols appeared inside the server application binary.

### Analysis of the server data output

The server container provided Netcat with command nc, enabling observation of the output from data ports 4001, 4002 and 4003 directly in the container shell. The data was observed together with timestamps to determine the varying signal periods.

``` sh
nc localhost 4001 | while IFS= read -r line; do printf '%s %s\n' "$(date +'%s')" "$line"; done
17092... 0.0
```

Also a TCP line rate_test utility was implemented to understand the rate and timing of the server TCP 4001 output:

``` bash
gcc -o utils/rate_test utils/rate_test.c
./utils/rate_test
Line 1: 0.030026 seconds
Line 2: 0.030031 seconds
Line 3: 0.040421 seconds
Line 4: 0.029793 seconds
Line 5: 0.010232 seconds
Line 6: 0.009262 seconds
Line 7: 0.040750 seconds
Line 8: 0.009129 seconds
Line 9: 0.010311 seconds
Line 10: 0.020608 seconds
...
```

### Probing of the control property fields

The control protocol operation, object, property, and value control fields were introduced without definition for the object and property fields, which requires some probing to figure out the necessary property indexes.

The server outputs control responses to stdout, appearing in the server logs, which gave an opportunity find out the property fields with a simple shell script: [utils/probe_properties.sh](utils/probe_properties.sh)

The properties were found by querying a property index range of 0 to 255, the start and first byte of the 16 bit property range, by simply observing if the property is found from the logs.

The set control properties correlate with the server data output values.

The frequency control index 255 property appeared to be set in mHz, correlating with observed cycle durations in the terminal, 500 mHz for 4001 and 250 mHz for 4002.

For 4003 there was no frequency property found in probed range 0-255. The were index 42 min_duration and index 43 max_duration properties, which correlated with the output 4003 observed binary waveform with varying signal period range.

The amplitude property control index 170 appeared to be the output amplitude multiplied by 1000, correlating with observed positive and negative amplitude in the terminal 4001, 4002.

For 4003 there was no amplitude property found in probed range of 0-255.

## Architecture

The implementation proposal is to split the project into a protocol module and into two assignment task specific client applications, the client1 and the client2.

The recognized common requirements from the assignment shall be introduced as features for the protocol and unique requirements as features for each of the client applications.

## Workflow

For each feature, a git branch is created for the implementation of a feature.

For each applicable feature, tests are introduced to validate correct operation.

Once a feature is implemented and tested, the feature branch is merged into the main branch and the main branch documentation updated.

### Build configuration

The [Makefile](Makefile) has been setup to build the client1 and client2 binaries and the binaries for the tests. The builds and tests can be run with:

``` bash
make
```

The application binaries can be built explicitly with:

``` bash
make client1
make client2
```

And the binaries cleaned with:

``` bash
make clean
```

### Run configuration

The applications can be run as ordinary command line applications after they are built. The applications provide the JSON reports to stdout, with client1:

``` bash
./client1
{"timestamp": ...}
```

And with client2:

``` bash
./client2
{"timestamp": ...}
```

#### Container configuration [TODO]

Added  [Dockerfile](Dockerfile) and [docker-compose.yml](docker-compose.yml) templates to support application deployment on container environments:

``` bash
docker network create internal
docker-compose up --build
docker-compose logs -f ctutorial
```

And to see the undisclosed server logs:

``` bash
docker-compose logs -f undisclosed_server
```

## Implementation [TODO]

The client1 and client2 applications have common functionality, which has been organized as the protocol module to be used by the client applications.

The protocol module provides functionality for data access, timing, reporting and control.

The functionality is tested by assertions on the function results and by windowing, comparing and inspecting specific expected outputs.

### Testing

A simple single header unit test mechanism has been included.

### Documentation [TODO]

As the project consist of several source files, an automated source documentation generator was considered practical.

The Doxygen generator configuration is found from [Doxyfile](Doxyfile).

Generate source documentation to [docs/html/index.html](docs/html/index.html) with:

´´´ bash
doxygen Doxyfile
...
´´´

### Protocol module [TODO]

The protocol module provides the data client, report printer and a property controller features.

Also utility functions for validation and to support testing have been implemented.

#### Data client

Read a TCP port.

- Linefeed \n character 0x10 separates the values in the TCP stream
- If no data available, return "--" as data value
- Non-blocking socket read for asynchronous operation
- Read socket buffer empty at end of receive time window

#### Property controller

Modulate the out1 amplitude and frequency properties based on out3 data value by sending control messages to UDP port 4000.

- Control UDP port 4000
- Control message fields are operation, object, property and value
- Control message fields are unsigned 16 bit integer
- Control message byte order is big-endian
- Frequency control property identifier is 255
- Frequency control property is scaled as mHz, e.g. property value 500 mHz results in 0.5 Hz output frequency
- Amplitude control property identifier is 170
- Amplitude control property property is multiplied by 1000, e.g. property value 1000 results in 1.0 output amplitude
- When out3 >= 3.0, set object out1 properties frequency to 1 Hz and amplitude to 8000
- When out3 < 3.0, set object out1 properties frequency to 2 Hz and amplitude to 4000
- Send message only when a valid value is received from the out3 and the out3 value crosses the control threshold

#### Report printer

Print JSON compatible ASCII reports to STDOUT at specified interval.

``` JSON
{"timestamp": 1709286246830, "out1": "-4.8", "out2": "8.0", "out3": "--"}
```

- Report Interval_ms, three TCP ports, control UDP socket and report count as arguments
- Runs until interrupted or report count reached 
- Report count can be set as infinite -1 or a positive integer
- Support 20 ms and 100 ms print interval
- Three TCP ports as out1, out2 and out3
- Millisecond timestamp
- Data values as the original data, no conversion to numeric representation
- No data aggregation
- No arrays in report
- Only report output on STDOUT
- Timer based to provide millisecond accuracy
- Finite report count support for testing

### client1 application [TODO]

- Report interval 100 ms
- Property controller disabled
- Terminate on SIGINT

### client2 application [TODO]

- Report interval 20 ms
- Property controller enabled
- Terminate on SIGINT

## License [TODO]

The project is open source and permissive MIT license was chosen for this effort to support for the open source community.

Please let me know, if there is an issue with the license, missing attribution or recognition of contribution.

See [LICENSE](LICENSE)
