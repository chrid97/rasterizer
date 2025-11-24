#!/bin/bash
set -euo pipefail

source ~/third-party/emsdk/emsdk_env.sh

mkdir -p build/

# emcc src/main.c -o ./build/index.html
emcc src/main.c \
  -s ERROR_ON_UNDEFINED_SYMBOLS=0 \
  -nostdlib \
  -Wl,--no-entry \
  -Wl,--export-all \
  -o build/index.wasm

cp ./src/main.js ./src/index.html ./build/
