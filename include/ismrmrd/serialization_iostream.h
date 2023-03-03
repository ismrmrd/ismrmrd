#pragma once

#include <iostream>
#include <ismrmrd/serialization.h>

namespace ISMRMRD {

class IStreamView : public ReadableStreamView {
public:
    IStreamView(std::istream &is) : _is(is) {}

    virtual void read(char *buffer, size_t count) {
        _is.read(buffer, count);
    }

    virtual bool eof() {
        return _is.eof();
    }

protected:
    std::istream &_is;
};

class OStreamView : public WritableStreamView {
public:
    OStreamView(std::ostream &os) : _os(os) {}

    void write(const char *buffer, size_t count) {
        _os.write(buffer, count);
    }

    bool bad() {
        return _os.bad();
    }

private:
    std::ostream &_os;
};

} // namespace ISMRMRD
