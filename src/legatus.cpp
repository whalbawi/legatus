#include "legatus.h"

#include "version.h"

namespace legatus {

std::string Legatus::get_version() {
    return "Version: " + version() + " - Commit: " + git_commit_hash();
}

void Legatus::add(int a, int b) {
    result = a + b;
}

int Legatus::get_result() const {
    return result;
}

} // namespace legatus
