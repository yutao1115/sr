#pragma once
#include <assert.h>

struct IndexAttr{
    enum InxType : unsigned {
        None = 0,
        Index16,
        Index32,
        NumIndexTypes,
        InvalidType = 0xFFFFFFFF
    };

    const unsigned Bytes[NumIndexTypes] = {
        0,
        2,
        4
    };

    unsigned getSize(InxType t){
        assert(t<NumIndexTypes);
        return Bytes[t];
    }

    unsigned indexType{None};
    unsigned numOfIndices{0};
    unsigned inxSize(){return IndexAttr::Bytes[indexType];}
    unsigned bytes(){ return numOfIndices*IndexAttr::Bytes[indexType]; }
};

