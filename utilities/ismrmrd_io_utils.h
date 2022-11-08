#pragma once

#ifdef _MSC_VER
#include <fcntl.h>
#include <io.h>
#endif

namespace ISMRMRD {
void set_binary_io() {
#ifdef _MSC_VER
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
#endif
}
} // namespace ISMRMRD
