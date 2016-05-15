#pragma once
#include <array>

struct VertexAttr{
// Vertex Attributes
    enum VASemantic {
        NO_SEMANTIC,
        POSITION,
        BLENDWEIGHT,
        BLENDINDICES,
        NORMAL,
        TEXCOORD0,
        TEXCOORD1,
        TEXCOORD2,
        TEXCOORD3,
        TANGENT,
        BINORMAL,
        TESSFACTOR,
        COLOR0,
        COLOR1,
        FOG,
        DEPTH,
        PSIZE,
        SAMPLE,
        NUM_SEMANTICS
    };

    enum VAConstant
    {
        // The maximum number of attributes for a vertex format.
        MAX_ATTRIBUTES   = 16,
        // The maximum number of texture coordinate units.
        MAX_TCOORD_UNITS = 4,
        // The maximum number of color units.
        MAX_COLOR_UNITS  = 2
    };

    struct Attribute{
        VASemantic semantic{NO_SEMANTIC};
        unsigned typeSize{0};
        unsigned offset{0};
    };
    using Container = std::array<Attribute,MAX_ATTRIBUTES>;
    inline bool add(VASemantic se,unsigned int size);
    unsigned int  bytes() const { return floatCnt_ * 4; }
    unsigned channels() const {return floatCnt_; }
    unsigned bytesOffset(VASemantic semantic){
        for(auto& i : attributes_){
            if(i.semantic == semantic)
                return i.offset;
        }
        return INVALIDBYTESOFFSET;
    }
    static constexpr unsigned INVALIDBYTESOFFSET = 0xFFFF;

    unsigned int size_{0};
    unsigned numOfAttrs_{0};
    unsigned floatCnt_{0};
    Container attributes_;
};



inline bool VertexAttr::add(VASemantic semantic,unsigned typesize){
    if (0 <= numOfAttrs_ && numOfAttrs_ < MAX_ATTRIBUTES) {

        Attribute& attribute = attributes_[numOfAttrs_];
        attribute.semantic   = semantic;
        attribute.typeSize   = (unsigned)typesize;
        attribute.offset     = size_;
        ++numOfAttrs_;
        size_ += attribute.typeSize;

        unsigned occupied = floatCnt_ * 4;
        if(size_ > occupied){
            unsigned cnt16byte=typesize<=4?1:
                ((typesize + (4-1)) & ~(4-1))/4;
          floatCnt_ += cnt16byte;
        }
        return true;
    }
    return false;
}

