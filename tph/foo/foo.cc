#include "tph/foo/foo.h"

#include <cstring>

namespace tph {

int FooFunc(const char *const str) {
  const auto len = std::strlen(str);
  return static_cast<int>(len);
}

} // namespace tph
