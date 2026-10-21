#!/bin/bash
set -euo pipefail
project_dir="$(cd "$(dirname "$0")/.." && pwd)"
sdk_path="$(xcrun --sdk macosx --show-sdk-path)"
cmake_bin="${CMAKE_BIN:-cmake}"
args=(-S "$project_dir" -B "$project_dir/build-macos"
      -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0
      "-DCMAKE_OSX_ARCHITECTURES=${COBALT_ARCHS:-arm64}"
      "-DCMAKE_OSX_SYSROOT=$sdk_path"
      "-DCMAKE_CXX_FLAGS=-nostdinc++ -isystem \"$sdk_path/usr/include/c++/v1\"")
if [[ -n "${JUCE_PATH:-}" ]]; then
    args+=("-DJUCE_PATH=$JUCE_PATH")
fi
"$cmake_bin" "${args[@]}"
"$cmake_bin" --build "$project_dir/build-macos" --config Release --parallel 4
ctest_bin="$(dirname "$(command -v "$cmake_bin")")/ctest"
"$ctest_bin" --test-dir "$project_dir/build-macos" -C Release --output-on-failure
