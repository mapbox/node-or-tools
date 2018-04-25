#!/usr/bin/env bash

set -eu
set -o pipefail

: '

Runs clang-format on the code in src/

Return `1` if there are files to be formatted, and automatically formats them.

Returns `0` if everything looks properly formatted.

'

PATH_TO_FORMAT_SCRIPT="$(pwd)/mason_packages/.link/bin/clang-format"

# Run clang-format on all cpp and hpp files in the /src directory
find src/ -type f -name '*.hpp' -o -name '*.cpp' -o -name '*.h' -o -name '*.cc' \
 | xargs -I{} ${PATH_TO_FORMAT_SCRIPT} -i -style=file {}

# Print list of modified files
dirty=$(git ls-files --modified src/)

if [[ $dirty ]]; then
    echo "The following files have been modified:"
    echo $dirty
    git diff
    exit 1
else
    exit 0
fi
