#include "legatus.h"

#include <string>

#include "version.h"

namespace legatus {

std::string Legatus::get_version() {
    return "Version: " + version() + " - Commit: " + git_commit_hash();
}

void Legatus::add(int a, int b) {
    result_ = a + b;
}

int Legatus::get_result() const {
    return result_;
}

} // namespace legatus
