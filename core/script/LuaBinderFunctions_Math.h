#ifndef _ITF_LUABINDEDFUNCTIONS_MATH_H_
#define _ITF_LUABINDEDFUNCTIONS_MATH_H_

struct lua_State;

#ifndef _ITF_VEC3D_H_
#include "core/math/Vec3d.h"
#endif //_ITF_VEC3D_H_

namespace ITF
{

Vec3d*  vectorPtrPop    (lua_State *L, int luaParam = 0);
int     vectorPop       (lua_State *L, Vec3d *vectorPtr, int luaParam = 0);
void    vectorPush      (lua_State *L, Vec3d *vectorPtr);
void    vectorPtrPush   (lua_State *L, Vec3d *vectorPtr);
int     vectorNew       (lua_State *L);

Vec2d*  vector2dPtrPop    (lua_State *L, int luaParam = 0);
int     vector2dPop       (lua_State *L, Vec2d *vectorPtr, int luaParam = 0);
void    vector2dPush      (lua_State *L, Vec2d *vectorPtr);
void    vector2dPtrPush   (lua_State *L, Vec2d *vectorPtr);
int     vector2dNew       (lua_State *L);
} // namespace ITF


#endif //_ITF_LUABINDEDFUNCTIONS_MATH_H_
