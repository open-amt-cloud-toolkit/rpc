<a name="v2.0.0"></a>
## v2.0.0

### Build
- bump version to v1.3.0
- bump version to v2.0.0

### Ci
- update scanning

### Docs
- **readme:** wording
- **readme:** update readme
- **security:** added SECURITY.md file

### Feat
- update to OpenSSL 1.1.1l
- update workflows and Docker build.
- Update to latest vcpkg
- Update docs and scripts for Windows and Ubuntu.
- format json status messages
- add AMT wireless adapter info to amtinfo LAN settings.

### Fix
- klockwork reported success / failure check fix
- **spelling:** lan inteface -> lan interface in amtinfo

<a name="v1.2.2"></a>
## [v1.2.2] - 2021-06-22
### Ci
- remove Jenkins chron
- **changelog:** add automation for changelog generation

### Fix
- update examples text and version

<a name="v1.2.1"></a>
## [v1.2.1] - 2021-05-06

### Fix
**docker:** add missing ca-certs

<a name="v1.2.0"></a>
## v1.2.0

### Ci
- breakout docker build for merge only

### Feat
- update RPC version to 1.2.0.
- BREAKING CHANGE: add heartbeat capability, bump RPC Protocol version to 4.0.0
- add unit test framework
- add hostname to activation info
- **docker:** add dockerfile support for RPC

### Fix
- use message status instead, cleanup message fields.


<a name="v1.1.0"></a>
## [v1.1.0] - 2021-02-09

### Build
- **jenkins:** jenkins build scripts for Windows and Ubuntu
- add support for centos7 and 8
- Use `-DNO_SELECT=ON` to work around select behavior on older distros
- Use vcpkg for both linux and windows

### Ci
- update build
- update build
- add support for release or debug
- add types for conventional commits
- add ci for status checks

### Docs
- add release disclaimer

### Fix
- link status not reported correctly
- free fqdn buffer

### Feat
- version update
- Add `--nocertcheck`, `-n` args to skip websocket server certificate verification for all builds.
- Add/update DNS Suffix (OS), Hostname (OS), fqdn [AMT] and DNS Suffix info returned by --amtinfo.

<a name="v1.0.0"></a>
## [v1.0.0] - 2020-11-20
### Build
- Add Github Actions Support

