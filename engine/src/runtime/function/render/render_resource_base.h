#pragma once

#include "render_type.h"
#include "tinyobjloader/tiny_obj_loader.h"

namespace Coconut
{
    class RHI;


    class RenderResourceBase
    {
    public:
     RenderResourceBase() = default;  // 让编译器自动生成
     virtual StaticMeshData loadStaticMesh(std::string filename) ;
     std::shared_ptr<TextureData> loadTexture(std::string filename);

     virtual ~RenderResourceBase();
     virtual void init()=0;


    };
}

