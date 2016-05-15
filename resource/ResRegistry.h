#pragma once
#include <stdint.h>
#include <forward_list>
#include <unordered_map>
#include "Id.h"
#include "Locator.h"


class ResRegistry{
public:
    static ResRegistry& instance(void);
    using Functor= std::function<void(const Id&)>;
    struct CallbackEntry{
        Functor factoryFunc;
        Functor releaseFunc;
    };


    void registerResType(uint16_t typee,
                         Functor create,
                         Functor release
                         ){
        regMap_.emplace(typee,CallbackEntry{create,release});
    }

    Id add(const Locator& loc,uint16_t type);
    void del(const Id& id);
    bool contains(const Id& id){return idEntryMap_.count(id) != 0;}
    const Locator& getLoc(const Id& id){
        if(contains(id)) return idEntryMap_[id]->locator_;
        else             return invalidDummy_;
    }


private:

    struct Entry {
        Entry(const Id& id,const Locator& loc)
        :id_(id),locator_(loc),pCall_(nullptr){}
        Id id_;
        Locator locator_;
        CallbackEntry* pCall_;
    };

    void createNew(const Locator& loc,uint16_t t);

    std::forward_list<Entry> entries_;
    std::unordered_map<Id,Entry*,Id::Hash,Id::Equal> idEntryMap_;
    std::unordered_map<std::string, Entry*> locEntryMap_;
    std::unordered_map<uint16_t,CallbackEntry> regMap_;

    static const Locator invalidDummy_;
};

