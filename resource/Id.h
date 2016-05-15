#pragma once
#include <stdint.h>
#include <functional> // hash
struct Id {
    typedef uint32_t UniqueStampT;
    typedef uint16_t SlotIndexT;
    typedef uint16_t TypeT;

    static const UniqueStampT InvalidUniqueStamp = 0xFFFFFFFF;
    static const SlotIndexT InvalidSlotIndex = 0xFFFF;
    static const TypeT InvalidType = 0xFFFF;
    static const uint64_t InvalidId = 0xFFFFFFFFFFFFFFFF;

    static Id invalidId(){return Id();}

    Id():value_(InvalidId){};

    Id(UniqueStampT uniqueStamp, SlotIndexT slotIndex, TypeT type)
	:slotIndex_(slotIndex),type_(type),uniqueStamp_(uniqueStamp){
    }

    bool isValid() const{return InvalidUniqueStamp != uniqueStamp_;}

    void invalidate(){ value_ = InvalidId;}
    bool operator==(const Id& rhs) const{return value_ == rhs.value_;}
    bool operator!=(const Id& rhs) const{return value_ != rhs.value_;}
    bool operator<(const Id& rhs) const{return value_ < rhs.value_;}

    struct Hash {
        std::size_t operator()(const Id& k) const {
            return std::hash<uint64_t>()(k.value_);
            }
    };

    struct Equal {
        bool operator()(const Id& lhs, const Id& rhs) const
            {
                return lhs.value_ == rhs.value_;
            }
    };


    union {
        struct {
            SlotIndexT slotIndex_;
            TypeT type_;
            UniqueStampT uniqueStamp_;
        };
        uint64_t value_;
    };
};
