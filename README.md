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

### Build configuration [TODO]
### Run configuration [TODO]
#### Container configuration [TODO]
## Implementation [TODO]
### Documentation [TODO]
### Protocol module [TODO]
#### Data client [TODO]
#### Property controller [TODO]
#### Report printer [TODO]
### client1 application [TODO]
### client2 application [TODO]
## License [TODO]
