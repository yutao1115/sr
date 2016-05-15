#include "ResourceState.h"
#include <string.h>

#define _FROMSTRING(code) if (0 == strcmp(#code, str)) return code

#define _TOSTRING(code) case code: return #code

const char* ResourceState::toString(Code c) {
    switch (c) {
        _TOSTRING(Initial);
        _TOSTRING(Setup);
        _TOSTRING(Pending);
        _TOSTRING(Valid);
        _TOSTRING(Failed);
    default: return "InvalidState";
    }
}

ResourceState::Code
ResourceState::fromString(const char* str) {
    _FROMSTRING(Initial);
    _FROMSTRING(Setup);
    _FROMSTRING(Pending);
    _FROMSTRING(Valid);
    _FROMSTRING(Failed);
    return InvalidState;
}
