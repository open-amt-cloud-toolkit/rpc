# Remote Provisioning Client (RPC)

RPC is an application which enables remote capabilities for AMT, such as as device activation. To accomplish this, RPC communicates with the RPS (Remote Provisioning Server).

The steps below assume the following directory structure where **rpc** is the clone of this repository, **vcpkg** is a clone of the VCPKG tool source and **build** is the RPC build directory. Both vcpkg and build directories will be created in later steps.

```
\rpc
  |__vcpkg
  |__build
```

# Linux

Steps below are for Ubuntu 18.04 and 20.04.

## Dependencies

```
sudo apt install git cmake build-essential curl zip unzip tar pkg-config
```

## Build C++ REST SDK

Open a Terminal window.

```
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg install cpprestsdk[websockets]
```

## Build RPC

Open a Terminal window.

```
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=/rpc/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

To build debug:
```
cmake -DCMAKE_TOOLCHAIN_FILE=/rpc/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

## Run RPC

Open a Terminal window.

```
cd build
sudo ./rpc --url wss://localhost:8080 --cmd "-t activate --profile profile1"
```

Use --help for more options.

# Windows

Steps below are for Windows 10 and Visual Studio 2019 Professional.

## Build C++ REST SDK

Open an x64 Native Tools Command Prompt for Visual Studio 2019.

```
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
bootstrap-vcpkg.bat
vcpkg install cpprestsdk[websockets]:x64-windows-static
```

## Build RPC
Open an x64 Native Tools Command Prompt for Visual Studio 2019.
```
mkdir build
cd build
cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static -DCMAKE_TOOLCHAIN_FILE=/rpc/vcpkg/scripts/buildsystems/vcpkg.cmake ..
cmake --build . --config Release
```

To build debug:
```
cmake --build . --config Debug
```

## Run RPC

Open a Command Prompt as Administrator.

```
cd build\Release
rpc.exe --url wss://localhost:8080 --cmd "-t activate --profile profile1"
```

Use --help for more options.

