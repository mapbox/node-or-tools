#!/usr/bin/env bash

set -eu
set -o pipefail

# Versions
MASON_RELEASE="or-tools-redux"
SPARSEHASH_RELEASE="2.0.2"
ORTOOLS_RELEASE="5.1"
PROTOBUF_RELEASE="3.0.0"
GFLAGS_RELEASE="2.1.2"

mkdir -p ./third_party

if [[ ! -d ./third_party/mason ]]; then
  mkdir -p ./third_party/mason
  echo "Downloading Mason @${MASON_RELEASE}"
  curl -sSfL https://github.com/mapbox/mason/archive/${MASON_RELEASE}.tar.gz | tar --gunzip --extract --strip-components=1 --directory=./third_party/mason
fi

./third_party/mason/mason install protobuf ${PROTOBUF_RELEASE}
./third_party/mason/mason link protobuf ${PROTOBUF_RELEASE}
./third_party/mason/mason install sparsehash ${SPARSEHASH_RELEASE}
./third_party/mason/mason link sparsehash ${SPARSEHASH_RELEASE}
./third_party/mason/mason install gflags ${GFLAGS_RELEASE}
./third_party/mason/mason link gflags ${GFLAGS_RELEASE}
./third_party/mason/mason install or-tools ${ORTOOLS_RELEASE}
./third_party/mason/mason link or-tools ${ORTOOLS_RELEASE}

mkdir -p build/Release/
cp mason_packages/.link/lib/libortools.* build/Release/
