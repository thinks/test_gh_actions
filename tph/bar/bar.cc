#include "tph/bar/bar.h"

#include <cstring>

namespace tph {

int BarFunc(const char *const str) {
  const auto len = std::strlen(str);
  return 42 - static_cast<int>(len);
}

} // namespace tph
