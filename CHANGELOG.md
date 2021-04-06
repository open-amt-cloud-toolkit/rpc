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

