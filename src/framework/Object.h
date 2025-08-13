//
// Created by tanhao on 2025/8/8.
//

#ifndef OBJECT_H
#define OBJECT_H
#include "Mesh.h"
#include "Reference.h"
#include "Shader.h"
#include "Texture.h"

#include <string>
#include <vector>

namespace ccn
{
class Object
{
	 Object() = default;
	~Object() = default;
};

class Object_3D
{
  public:
	 Object_3D() = default;
	~Object_3D() = default;

  public:
	Ref::share<Mesh>    mesh;
	Ref::share<Texture> texture;
	Ref::share<Shader>  shader;
};

}        // namespace ccn
#endif        // OBJECT_H
