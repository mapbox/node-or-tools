#!/usr/bin/env bash

set -eu
set -o pipefail

: '

Rebuilds the code with the sanitizers and runs the tests

'

# See https://github.com/mapbox/node-cpp-skel/blob/master/docs/extended-tour.md#configuration-files

make clean

# https://github.com/google/sanitizers/wiki/AddressSanitizerAsDso
SHARED_LIB_EXT=.so
if [[ $(uname -s) == 'Darwin' ]]; then
    SHARED_LIB_EXT=.dylib
fi

if [[ $(uname -s) == 'Linux' ]]; then
    ./node_modules/.bin/mason-js install binutils=2.30 --type=compiled
    ./node_modules/.bin/mason-js link binutils=2.30 --type=compiled
    export PATH=$(pwd)/mason_packages/.link/bin:${PATH}
fi
export MASON_LLVM_RT_PRELOAD=$(pwd)/$(ls mason_packages/.link/lib/clang/*/lib/*/libclang_rt.asan*${SHARED_LIB_EXT})
SUPPRESSION_FILE="/tmp/leak_suppressions.txt"
echo "leak:__strdup" > ${SUPPRESSION_FILE}
echo "leak:v8::internal" >> ${SUPPRESSION_FILE}
echo "leak:node::CreateEnvironment" >> ${SUPPRESSION_FILE}
echo "leak:node::Init" >> ${SUPPRESSION_FILE}
export ASAN_SYMBOLIZER_PATH=$(pwd)/mason_packages/.link/bin/llvm-symbolizer
export MSAN_SYMBOLIZER_PATH=$(pwd)/mason_packages/.link/bin/llvm-symbolizer
export UBSAN_OPTIONS=print_stacktrace=1
export LSAN_OPTIONS=suppressions=${SUPPRESSION_FILE}
export ASAN_OPTIONS=detect_leaks=1:symbolize=1:abort_on_error=1:detect_container_overflow=1:check_initialization_order=1:detect_stack_use_after_return=1
export MASON_SANITIZE="-fsanitize=address,undefined,integer,leak -fno-sanitize=vptr,function"
export MASON_SANITIZE_CXXFLAGS="${MASON_SANITIZE} -fno-sanitize=vptr,function -fsanitize-address-use-after-scope -fno-omit-frame-pointer -fno-common"
export MASON_SANITIZE_LDFLAGS="${MASON_SANITIZE}"
# Note: to build without stopping on errors remove the -fno-sanitize-recover=all flag
# You might want to do this if there are multiple errors and you want to see them all before fixing
export CXXFLAGS="${MASON_SANITIZE_CXXFLAGS} ${CXXFLAGS:-} -fno-sanitize-recover=all"
export LDFLAGS="${MASON_SANITIZE_LDFLAGS} ${LDFLAGS:-}"
make debug
export ASAN_OPTIONS=fast_unwind_on_malloc=0:${ASAN_OPTIONS}
if [[ $(uname -s) == 'Darwin' ]]; then
    # NOTE: we must call node directly here rather than `npm test`
    # because OS X blocks `DYLD_INSERT_LIBRARIES` being inherited by sub shells
    # If this is not done right we'll see
    #   ==18464==ERROR: Interceptors are not working. This may be because AddressSanitizer is loaded too late (e.g. via dlopen).
    #
    DYLD_INSERT_LIBRARIES=${MASON_LLVM_RT_PRELOAD} \
      node node_modules/.bin/tape test/*test.js
else
    LD_PRELOAD=${MASON_LLVM_RT_PRELOAD} \
      npm test
fi

