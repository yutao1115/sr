#include "ResRegistry.h"
#include <time.h>

static unsigned int s_sequence;

static unsigned int nextSeq(){
    return s_sequence++;
}
const Locator ResRegistry::invalidDummy_("");

ResRegistry& ResRegistry::instance(void){
    static ResRegistry  s_reg;
    return s_reg;
}

void ResRegistry::createNew(const Locator& loc,uint16_t t){
    Id id(time(0),nextSeq(),t);
    entries_.emplace_front(id,loc);
    auto& entry = entries_.front();
    idEntryMap_[id] = &entry;
    locEntryMap_[loc.location_] = &entry;
    
    // default callback
    entry.pCall_ = &regMap_[id.type_];
    entry.pCall_->factoryFunc(id);
}

Id ResRegistry::add(const Locator& loc,uint16_t t){
    if(! loc.isShared()){
        createNew(loc,t);
        return entries_.front().id_;
    }
    if( locEntryMap_.count(loc.location_) ){
        return locEntryMap_[loc.location_]->id_;
    }

    createNew(loc,t);
    return entries_.front().id_;
}

void ResRegistry::del(const Id& id){
    auto i = idEntryMap_.find(id);
    if( i != idEntryMap_.end() ){
        Entry* entry = i->second;
        locEntryMap_.erase(entry->locator_.location_);
        idEntryMap_.erase(id);
        entry->pCall_->releaseFunc(id);
        entries_.remove_if([id](const Entry& m) {return m.id_ == id; });
    }
}

