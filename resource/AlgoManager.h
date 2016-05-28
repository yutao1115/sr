#pragma once
 
#include "Algo.h"
#include <unordered_map>
#include <vector>
#include <stdint.h>

struct  Mesh;
template<class IAgo>
class AlgoManager{
public:
    static AlgoManager& instance(void){
        static AlgoManager manager;
        return manager;
    }
    
    AlgoManager( const AlgoManager& ) = delete;
    AlgoManager& operator=( const AlgoManager& ) = delete;

    void registerSameAs(Mesh*thatMesh,Mesh*mesh){
        algoMap_.emplace(mesh,get(thatMesh));
    }
    
    void registerSameAs(int index,Mesh*mesh){
        algoMap_.emplace(mesh,&alogs_[index]);
    }
    
    uint32_t registerMesh(Mesh* mesh){
        alogs_.emplace_back();
        algoMap_[mesh] = &alogs_.back();
        return alogs_.size()-1;
    }
    
    IAgo* get(Mesh* m){ return ((algoMap_.count(m))>0) ? algoMap_[m]:nullptr; }

private:
    AlgoManager(){}
    std::unordered_map<Mesh*,IAgo*> algoMap_;
    std::vector<IAgo> alogs_;
};
extern template class AlgoManager<Interpolat>;
extern template class AlgoManager<IScanline>;

