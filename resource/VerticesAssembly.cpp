#include "VerticesAssembly.h"
#include "Mesh.h"
#include "VertexAttr.h"
#include "AlgoManager.h"

void VerticesAssembly::init(Mesh*vt){
    attr = vt->va_.get(); 
    base = AlgoManager<IBase>::instance().get(vt);
    hasOtherChannels = (attr->channels()>4);
}

