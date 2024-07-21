# ctutorial

C tutorial with protocols, real time signal analysis and control implementation, including workspace and build configuration

## Author

Jani Yli-Alho

- [https://github.com/jylialho](https://github.com/jylialho)
- [https://www.linkedin.com/in/jylialho/](https://www.linkedin.com/in/jylialho/)

## Assignment summary [TODO]
### Task 1 results [TODO]
#### Server TCP port signal frequencies, amplitudes and shapes [TODO]
### Task 2 results [TODO]
#### Control protocol [TODO]
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
git clone https://github.com/jylialho/ctutorial.git
```

#### Development container [TODO]
## Server container deployment [TODO]
### Analysis of the server data output [TODO]
### Probing of the control property fields [TODO]
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
#### Data client [TODO]
#### Property controller [TODO]
#### Report printer [TODO]
### client1 application [TODO]
### client2 application [TODO]
## License [TODO]
