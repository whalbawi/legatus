#ifndef LEGATUS_LEGATUS_H_
#define LEGATUS_LEGATUS_H_

#include <string>

namespace legatus {

class Legatus {
  public:
    static std::string get_version();

    void add(int a, int b);
    int get_result() const;

  private:
    int result;
};

} // namespace legatus

#endif // LEGATUS_LEGATUS_H_
