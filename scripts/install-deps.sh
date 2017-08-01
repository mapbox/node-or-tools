#!/usr/bin/env bash

set -eu
set -o pipefail

# Point to or-tools upstream mason package commit until we have a
# new mason release. See https://github.com/mapbox/mason/pull/426
MASON_RELEASE="ca9b4cb"
SPARSEHASH_RELEASE="2.0.2"
ORTOOLS_RELEASE="6.0"
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


# We ship debug binaries with mason but for production builds we just strip debug symbols out.
# In case you need debug symbols just comment out the following line and `npm --build-from-source`.
find mason_packages/ -type f -name 'libortools.*' -exec strip --strip-unneeded {} \;
