#pragma once

#include <string>

namespace legatus {

class Legatus {
  public:
    static std::string get_version();

    void add(int a, int b);
    int get_result() const;

  private:
    int result_;
};

} // namespace legatus
