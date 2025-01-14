#include "axle.h"

#include <string>

#include "version.h"

namespace axle {

std::string Axle::get_version() {
    return "Version: " + version() + " - Commit: " + git_commit_hash();
}

void Axle::add(int a, int b) {
    result_ = a + b;
}

int Axle::get_result() const {
    return result_;
}

} // namespace axle
