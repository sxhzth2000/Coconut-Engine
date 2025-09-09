#pragma once
namespace Coconut
{
    class RHI;


    class RenderResourceBase
    {
    public:
     virtual ~RenderResourceBase();
     virtual void init()=0;


    };
}

