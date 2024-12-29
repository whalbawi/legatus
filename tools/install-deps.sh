#!/usr/bin/env bash

SRC_DIR=$(dirname ${BASH_SOURCE[0]})
OS=$(uname)
if [ $OS = "Darwin" ]; then
    echo "Running on macOS"
    . $SRC_DIR/setup-darwin.sh
else
    echo "Unsupported OS: $OS"
    exit 1
fi

install-clang
install-clang-format
install-clang-tidy
