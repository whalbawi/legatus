function install-clang-format() {
    brew update
    brew install clang-format@19
}

function install-clang-tidy() {
    brew update
    brew install llvm@19
    sudo ln -s "$(brew --prefix llvm@19)/bin/clang-tidy" "/usr/local/bin/clang-tidy"
}
