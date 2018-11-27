#!/usr/bin/env bash

set -eu
set -o pipefail

MASON_RELEASE="v0.19.0"
SPARSEHASH_RELEASE="2.0.2"
ORTOOLS_RELEASE="6.0"
PROTOBUF_RELEASE="3.0.0"
GFLAGS_RELEASE="2.1.2"

mkdir -p ./third_party

if [[ ! -d ./third_party/mason ]]; then
  mkdir -p ./third_party/mason
  echo "Downloading Mason @${MASON_RELEASE}"
  curl -vsSfL https://github.com/mapbox/mason/archive/${MASON_RELEASE}.tar.gz | tar --gunzip --extract --strip-components=1 --directory=./third_party/mason
  echo "Mason @${MASON_RELEASE} unpacked to ./third_party/mason"
fi

echo "Installing and linking Mason deps"
./third_party/mason/mason install protobuf ${PROTOBUF_RELEASE}
./third_party/mason/mason link protobuf ${PROTOBUF_RELEASE}
./third_party/mason/mason install sparsehash ${SPARSEHASH_RELEASE}
./third_party/mason/mason link sparsehash ${SPARSEHASH_RELEASE}
./third_party/mason/mason install gflags ${GFLAGS_RELEASE}
./third_party/mason/mason link gflags ${GFLAGS_RELEASE}
./third_party/mason/mason install or-tools ${ORTOOLS_RELEASE}
./third_party/mason/mason link or-tools ${ORTOOLS_RELEASE}
echo "Done installing and linking Mason deps"


# We ship debug binaries with mason but for production builds we just strip debug symbols out.
# In case you need debug symbols just comment out the following line and `npm --build-from-source`.
if [ "$(uname -s)" == 'Darwin' ]; then
  STRIP_CMD="strip -x"
else
  STRIP_CMD="strip --strip-unneeded"
fi
pwd
for path in $(find $(pwd)/mason_packages -type f -name 'libortools.*'); do
   echo "stripping debug symbols from ${path}"
   $STRIP_CMD $path
done
