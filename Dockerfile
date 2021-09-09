#*********************************************************************
# Copyright (c) Intel Corporation 2021
# SPDX-License-Identifier: Apache-2.0
#*********************************************************************/

FROM ubuntu:20.04 AS rpc-builder

WORKDIR /
ARG DEBIAN_FRONTEND=noninteractive
RUN \
  apt-get update -y -qq && \
  apt install -y -qq \
    git cmake build-essential libssl-dev zlib1g-dev \
    curl unzip zip pkg-config ca-certificates
RUN git clone https://github.com/open-amt-cloud-toolkit/rpc.git
WORKDIR /rpc
RUN mkdir -p build
RUN git clone https://github.com/microsoft/vcpkg.git && cd vcpkg && git checkout 772d435ba18bf2f342458e0187ab7b48b84fe3f0
RUN cd vcpkg && ./bootstrap-vcpkg.sh
RUN ./vcpkg/vcpkg install cpprestsdk[websockets]

WORKDIR /rpc/build
RUN cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=/rpc/vcpkg/scripts/buildsystems/vcpkg.cmake ..
RUN cmake --build .

FROM ubuntu:20.04 

LABEL license='SPDX-License-Identifier: Apache-2.0' \
      copyright='Copyright (c) 2021: Intel'

WORKDIR /root
RUN \
  apt-get update -y -qq && \
  apt install -y -qq \
    libssl-dev
COPY --from=rpc-builder /rpc/build/rpc .
ENTRYPOINT ["/root/rpc"]