#include "Mesh.h"
#include "Enum.h"
#include "ResRegistry.h"
#include <unordered_map>
#include <forward_list>
#include <algorithm>

struct MeshFactory{
    static MeshFactory& instance(){static MeshFactory f;return f;}
    static void factory(const Id& id);
    static void release(const Id& id);
    static std::forward_list<Mesh> meshMemory;
    static std::unordered_map<Id,Mesh*,Id::Hash,Id::Equal> meshMap;

    struct Init{
        Init(){
            ResRegistry::instance()
                .registerResType(ResourceType::Mesh_T,
                                 MeshFactory::factory,
                                 MeshFactory::release);
        }
    }inter__;
};

std::forward_list<Mesh> MeshFactory::meshMemory;
std::unordered_map<Id,Mesh*,Id::Hash,Id::Equal> MeshFactory::meshMap;

namespace {
    struct mfInit{
        mfInit(){MeshFactory::instance();}
    }k;
}


using InxType = IndexAttr::InxType;
using PrimitiveT = Mesh::PrimitiveT;

std::shared_ptr<Mesh> Mesh::resolveId(const Id& id){
    if(MeshFactory::meshMap.count(id))
        return std::shared_ptr<Mesh> (
            MeshFactory::meshMap[id],
          [](Mesh* m){
        if(m) ResRegistry::instance().del(m->id_);});
    else
        return nullptr;
}
void MeshFactory::factory(const Id& id) {
    meshMemory.emplace_front();
    meshMemory.front().id_ = id;
    meshMap.emplace(id, &meshMemory.front());
}

void MeshFactory::release(const Id& id) {
    meshMemory.remove_if([id](const Mesh& m) {return m.id_ == id; });
}
