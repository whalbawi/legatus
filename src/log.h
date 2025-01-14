#pragma once

#include <format>
#include <iostream>

namespace axle {

template <typename... Args>

// NOLINTNEXTLINE(misc-include-cleaner) -- for std::format_string
void log(std::format_string<Args...> fmt, Args&&... args) {
    std::cerr << std::format(fmt, std::forward<Args>(args)...);
}

} // namespace axle
