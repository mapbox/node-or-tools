# This Makefile serves a few purposes:
#
# 1. It provides an interface to iterate quickly while developing the C++ code in src/
# by typing `make` or `make debug`. To make iteration as fast as possible it calls out
# directly to underlying build tools and skips running steps that appear to have already
# been run (determined by the presence of a known file or directory). What `make` does is
# the same as running `npm install --build-from-source` except that it is faster when
# run a second time because it will skip re-running expensive steps.
# Note: in rare cases (like if you hit `crtl-c` during an install) you might end up with
# build deps only partially installed. In this case you should run `make distclean` to fully
# restore your repo to is starting state and then running `make` again should start from
# scratch, fixing any inconsistencies.
#
# 2. It provides a few commands that call out to external scripts like `make coverage` or
# `make tidy`. These scripts can be called directly but this Makefile provides a more uniform
# interface to call them.
#
# To learn more about the build system see https://github.com/mapbox/node-cpp-skel/blob/master/docs/extended-tour.md#builds

# Whether to turn compiler warnings into errors
export WERROR ?= true

# the default target. This line means that
# just typing `make` will call `make release`
default: release

node_modules/nan:
	npm install --ignore-scripts

mason_packages/headers: node_modules/nan
	node_modules/.bin/mason-js install

mason_packages/.link/include: mason_packages/headers
	node_modules/.bin/mason-js link

build-deps: mason_packages/.link/include

release: build-deps
	V=1 ./node_modules/.bin/node-pre-gyp configure build --error_on_warnings=$(WERROR) --loglevel=error
	@echo "run 'make clean' for full rebuild"

debug: mason_packages/.link/include
	V=1 ./node_modules/.bin/node-pre-gyp configure build --error_on_warnings=$(WERROR) --loglevel=error --debug
	@echo "run 'make clean' for full rebuild"

coverage: build-deps
	./scripts/coverage.sh

tidy: build-deps
	./scripts/clang-tidy.sh

format: build-deps
	./scripts/clang-format.sh

sanitize: build-deps
	./scripts/sanitize.sh

clean:
	rm -rf lib/binding
	rm -rf build
	# remove remains from running 'make coverage'
	rm -f *.profraw
	rm -f *.profdata
	@echo "run 'make distclean' to also clear node_modules, mason_packages, and .mason directories"

distclean: clean
	rm -rf node_modules
	rm -rf mason_packages

# variable used in the `xcode` target below
MODULE_NAME := $(shell node -e "console.log(require('./package.json').binary.module_name)")

xcode: node_modules
	./node_modules/.bin/node-pre-gyp configure -- -f xcode
	@# If you need more targets, e.g. to run other npm scripts, duplicate the last line and change NPM_ARGUMENT
	SCHEME_NAME="$(MODULE_NAME)" SCHEME_TYPE=library BLUEPRINT_NAME=$(MODULE_NAME) BUILDABLE_NAME=$(MODULE_NAME).node scripts/create_scheme.sh
	SCHEME_NAME="npm test" SCHEME_TYPE=node BLUEPRINT_NAME=$(MODULE_NAME) BUILDABLE_NAME=$(MODULE_NAME).node NODE_ARGUMENT="`npm bin tape`/tape test/*.test.js" scripts/create_scheme.sh

	open build/binding.xcodeproj

testpack:
	rm -f ./*tgz
	npm pack
	tar -ztvf *tgz

docs:
	npm run docs

test:
	npm test

.PHONY: test docs
