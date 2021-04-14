#*********************************************************************
# Copyright (c) Intel Corporation 2021
# SPDX-License-Identifier: Apache-2.0
#*********************************************************************/

FROM ubuntu:groovy-20210325 AS rpc-builder

WORKDIR /
ARG DEBIAN_FRONTEND=noninteractive
RUN \
  apt-get update -y -qq && \
  apt install -y -qq \
    git cmake build-essential libssl-dev zlib1g-dev \
    curl unzip zip pkg-config
RUN git clone https://github.com/open-amt-cloud-toolkit/rpc.git
WORKDIR /rpc
RUN mkdir -p build
RUN git clone --branch 2020.11-1 https://github.com/microsoft/vcpkg.git
RUN cd vcpkg && ./bootstrap-vcpkg.sh
RUN ./vcpkg/vcpkg install cpprestsdk[websockets]

WORKDIR /rpc/build
RUN cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=/rpc/vcpkg/scripts/buildsystems/vcpkg.cmake ..
RUN cmake --build .

FROM ubuntu:groovy-20210325 

LABEL license='SPDX-License-Identifier: Apache-2.0' \
      copyright='Copyright (c) 2021: Intel'

WORKDIR /root
RUN \
  apt-get update -y -qq && \
  apt install -y -qq \
    libssl-dev
COPY --from=rpc-builder /rpc/build/rpc .
ENTRYPOINT ["/root/rpc"]