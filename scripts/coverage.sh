#!/usr/bin/env bash

set -x
set -eu
set -o pipefail

# http://clang.llvm.org/docs/UsersManual.html#profiling-with-instrumentation
# https://www.bignerdranch.com/blog/weve-got-you-covered/

make clean
export CXXFLAGS="-fprofile-instr-generate -fcoverage-mapping -coverage"
export LDFLAGS="-fprofile-instr-generate -coverage"
make debug
rm -f *profraw
rm -f *gcov
rm -f *profdata
LLVM_PROFILE_FILE="code-%p.profraw" npm test
CXX_MODULE=$(./node_modules/.bin/node-pre-gyp reveal module --silent)
export PATH=$(pwd)/mason_packages/.link/bin/:${PATH}
llvm-profdata merge -output=code.profdata code-*.profraw
llvm-cov report ${CXX_MODULE} -instr-profile=code.profdata -use-color
llvm-cov show ${CXX_MODULE} -instr-profile=code.profdata src/*.cpp src/*.cc -filename-equivalence -use-color
#llvm-cov show ${CXX_MODULE} -instr-profile=code.profdata src/*.cpp src/*.cc -filename-equivalence -use-color --format html > /tmp/coverage.html
#echo "open /tmp/coverage.html for HTML version of this report"
