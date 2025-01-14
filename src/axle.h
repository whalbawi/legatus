#pragma once

#include <string>

namespace axle {

class Axle {
  public:
    static std::string get_version();

    void add(int a, int b);
    int get_result() const;

  private:
    int result_;
};

} // namespace axle
