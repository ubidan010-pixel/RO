#include "precompiled_core.h"

#ifndef _ITF_LUAHANDLER_H_
#include "core/script/LUAHandler.h"
#endif //_ITF_LUAHANDLER_H_

#ifndef _ITF_LUABINDEDFUNCTIONS_MATH_H_
#include "LuaBinderFunctions_Math.h"
#endif //_ITF_LUABINDEDFUNCTIONS_MATH_H_

extern "C" 
{
#include "lua/src/lua.h"
#include "lua/src/lualib.h"
#include "lua/src/lauxlib.h"
}



namespace ITF
{

int     vectorClone(lua_State *L);
int     vectorAdd(lua_State *L);
int     vectorSub(lua_State *L);
int     vectorMul(lua_State *L);
int     vectorPow(lua_State *L);
int     vectorSqrNorm(lua_State *L);
int     vectorNorm(lua_State *L);
int     vectorNormalize(lua_State *L);
void    vectorRegister(lua_State *L);
void    vectorPtrRegister(lua_State *L);

int     vector2dClone(lua_State *L);
int     vector2dAdd(lua_State *L);
int     vector2dSub(lua_State *L);
int     vector2dMul(lua_State *L);
int     vector2dPow(lua_State *L);
int     vector2dSqrNorm(lua_State *L);
int     vector2dNorm(lua_State *L);
int     vector2dNormalize(lua_State *L);
void    vector2dRegister(lua_State *L);
void    vector2dPtrRegister(lua_State *L);

void LUAHandler::registerBindedFunctions_Math()
{
    vectorRegister(L);
    vectorPtrRegister(L);
    vector2dRegister(L);
    vector2dPtrRegister(L);

    REGISTER("vectorNew", vectorNew);
    REGISTER("vector2dNew", vector2dNew);
#ifdef CAMCAM_COMPATIBILITY
    lua_register(L, "VECTOR_New", vectorNew);
#endif 
}



Vec3d *vectorPtrPop(lua_State *L, int luaParam)
{
    lua_gettop(L);
    void    *p        = lua_touserdata(L, luaParam);
    Vec3d    *vPtr    = NULL;

    if (p == NULL)                        return NULL;
    if (!lua_getmetatable(L, luaParam))    return NULL;

    lua_getfield(L, LUA_REGISTRYINDEX, "vector");
    if (lua_rawequal(L, -1, -2))
        vPtr = (Vec3d *)p;
    else
    {
        lua_pop(L, 1);
        lua_getfield(L, LUA_REGISTRYINDEX, "vectorPtr");
        if (lua_rawequal(L, -1, -2))
            vPtr = *((Vec3d **)p);
    }
    lua_pop(L, 2);
    return vPtr;
}

Vec2d *vector2dPtrPop(lua_State *L, int luaParam)
{
    lua_gettop(L);
    void    *p        = lua_touserdata(L, luaParam);
    Vec2d    *vPtr    = NULL;

    if (p == NULL)                        return NULL;
    if (!lua_getmetatable(L, luaParam))    return NULL;

    lua_getfield(L, LUA_REGISTRYINDEX, "vector2d");
    if (lua_rawequal(L, -1, -2))
        vPtr = (Vec2d *)p;
    else
    {
        lua_pop(L, 1);
        lua_getfield(L, LUA_REGISTRYINDEX, "vector2dPtr");
        if (lua_rawequal(L, -1, -2))
            vPtr = *((Vec2d **)p);
    }
    lua_pop(L, 2);
    return vPtr;
}

int vectorPop(lua_State *L, Vec3d *vectorPtr, int luaParam)
{
    Vec3d *vPtr = vectorPtrPop(L, luaParam);
    if (vPtr == NULL)                            return 0;
    ITF_Memcpy(vectorPtr, vPtr, sizeof(Vec3d));    return 1;
}    

int vector2dPop(lua_State *L, Vec2d *vectorPtr, int luaParam)
{
    Vec2d *vPtr = vector2dPtrPop(L, luaParam);
    if (vPtr == NULL)                            return 0;
    ITF_Memcpy(vectorPtr, vPtr, sizeof(Vec2d));    return 1;
}    

void vectorPush(lua_State *L, Vec3d *vectorPtr)
{
    void *VectorUser = lua_newuserdata(L, sizeof(Vec3d));
    if (!VectorUser) return;

    ITF_Memcpy(VectorUser, vectorPtr, sizeof(Vec3d));
    luaL_getmetatable(L, "vector");
    lua_setmetatable (L, -2);
}

void vector2dPush(lua_State *L, Vec2d *vectorPtr)
{
    void *VectorUser = lua_newuserdata(L, sizeof(Vec2d));
    if (!VectorUser) return;

    ITF_Memcpy(VectorUser, vectorPtr, sizeof(Vec2d));
    luaL_getmetatable(L, "vector2d");
    lua_setmetatable (L, -2);
}

void vectorPush(lua_State *L, char *vectorString)
{
    Vec3d *vectorPtr = new Vec3d();
    if(sscanf(vectorString,"{x=%f,y=%f,z=%f}", &vectorPtr->m_x, &vectorPtr->m_y, &vectorPtr->m_z)==3)
        vectorPush(L,vectorPtr);
}

void vector2dPush(lua_State *L, char *vectorString)
{
    Vec2d *vectorPtr = new Vec2d();
    if(sscanf(vectorString,"{x=%f,y=%f}", &vectorPtr->m_x, &vectorPtr->m_y)==2)
        vector2dPush(L,vectorPtr);
}

void    vectorPtrPush(lua_State *L, Vec3d *vectorPtr)
{
    void *VectorUserPtr = lua_newuserdata(L, sizeof(Vec3d *));
    if (!VectorUserPtr) return;

    *((Vec3d **)VectorUserPtr) = vectorPtr;
    luaL_getmetatable(L, "vectorPtr");
    lua_setmetatable (L, -2);
}

void    vector2dPtrPush(lua_State *L, Vec2d *vectorPtr)
{
    void *VectorUserPtr = lua_newuserdata(L, sizeof(Vec2d *));
    if (!VectorUserPtr) return;

    *((Vec2d **)VectorUserPtr) = vectorPtr;
    luaL_getmetatable(L, "vector2dPtr");
    lua_setmetatable (L, -2);
}

int vectorIndex(lua_State *L)
{
    Vec3d *vPtr = vectorPtrPop(L, 1);

    if (!vPtr)
        return 0;

    const char *tableStr = luaL_checkstring(L, 2);
    if (tableStr[1] == 0)
    {
        switch (*tableStr)
        {
        case 'x':    lua_pushnumber(L, vPtr->m_x);        return 1;
        case 'y':    lua_pushnumber(L, vPtr->m_y);        return 1;
        case 'z':    lua_pushnumber(L, vPtr->m_z);        return 1;
        }
        return 0;
    }
    if (!strcmp(tableStr, "Norm"))
    {
        lua_pushcfunction(L, vectorNorm);
        return 1;
    }
    if (!strcmp(tableStr, "SqrNorm"))
    {
        lua_pushcfunction(L, vectorSqrNorm);
        return 1;
    }
    if (!strcmp(tableStr, "Normalize"))
    {
        lua_pushcfunction(L, vectorNormalize);
        return 1;
    }
    if (!strcmp(tableStr, "Clone"))
    {
        lua_pushcfunction(L, vectorClone);
        return 1;
    }
    return 0;
}


int vector2dIndex(lua_State *L)
{
    Vec2d *vPtr = vector2dPtrPop(L, 1);

    if (!vPtr)
        return 0;

    const char *tableStr = luaL_checkstring(L, 2);
    if (tableStr[1] == 0)
    {
        switch (*tableStr)
        {
        case 'x':    lua_pushnumber(L, vPtr->m_x);        return 1;
        case 'y':    lua_pushnumber(L, vPtr->m_y);        return 1;
        }
        return 0;
    }
    if (!strcmp(tableStr, "Norm"))
    {
        lua_pushcfunction(L, vector2dNorm);
        return 1;
    }
    if (!strcmp(tableStr, "SqrNorm"))
    {
        lua_pushcfunction(L, vector2dSqrNorm);
        return 1;
    }
    if (!strcmp(tableStr, "Normalize"))
    {
        lua_pushcfunction(L, vector2dNormalize);
        return 1;
    }
    if (!strcmp(tableStr, "Clone"))
    {
        lua_pushcfunction(L, vector2dClone);
        return 1;
    }
    return 0;
}

int vectorNewIndex(lua_State *L)
{
    Vec3d *vPtr = vectorPtrPop(L, 1);

    if (!vPtr)
        return 0;

    const char *tableStr    = luaL_checkstring(L, 2);
    float number            = (float)luaL_checknumber(L, 3);
    if (tableStr[1] != 0)            return 0;

    switch (*tableStr)
    {
    case 'x':    vPtr->m_x = number;    return 1;
    case 'y':    vPtr->m_y = number;    return 1;
    case 'z':    vPtr->m_z = number;    return 1;
    }
    return 0;
}

int vector2dNewIndex(lua_State *L)
{
    Vec2d *vPtr = vector2dPtrPop(L, 1);

    if (!vPtr)
        return 0;

    const char *tableStr    = luaL_checkstring(L, 2);
    float number            = (float)luaL_checknumber(L, 3);

    if (tableStr[1] != 0)
        return 0;

    switch (*tableStr)
    {
    case 'x':    vPtr->m_x = number;    return 1;
    case 'y':    vPtr->m_y = number;    return 1;
    }
    return 0;
}


int vectorClone(lua_State *L)
{
    Vec3d    *vPtr = vectorPtrPop(L, 1);
    if (!vPtr)        return 0;
    vectorPush(L, vPtr);
    return 1;
}


int vector2dClone(lua_State *L)
{
    Vec2d    *vPtr = vector2dPtrPop(L, 1);
    if (!vPtr)        return 0;
    vector2dPush(L, vPtr);
    return 1;
}

int vectorToString(lua_State *L)
{
    Vec3d    *vPtr = vectorPtrPop(L, 1);
    char    vStr[256];
    
    if (!vPtr)
        return 0;    
    
    sprintf(vStr,    "{x=%.2f,y=%.2f,z=%.2f}",
                    vPtr->m_x, vPtr->m_y, vPtr->m_z);
    lua_pushstring(L, vStr);
    return 1;
}

int vector2dToString(lua_State *L)
{
    Vec2d    *vPtr = vector2dPtrPop(L, 1);
    char    vStr[256];

    if (!vPtr)
        return 0;    

    sprintf(vStr,    "{x=%.2f,y=%.2f}",
        vPtr->m_x, vPtr->m_y);
    lua_pushstring(L, vStr);
    return 1;
}

int vectorAdd(lua_State *L)
{
    Vec3d v1, *v2Ptr = NULL;

    if (!vectorPop(L, &v1, 1))
        return 0;
    
    if (lua_isnumber(L, 2))
    {
        v1 += (float)lua_tonumber(L, -1);
        vectorPush(L, &v1);
        return 1;
    }

    if ((v2Ptr = vectorPtrPop(L, 2)) != NULL)
    {
        v1 = v1+*v2Ptr;
        vectorPush(L, &v1);
        return 1;
    }
    return 0;
}

int vector2dAdd(lua_State *L)
{
    Vec2d v1, *v2Ptr = NULL;

    if (!vector2dPop(L, &v1, 1))
        return 0;

    if (lua_isnumber(L, 2))
    {
        v1 += (float)lua_tonumber(L, -1);
        vector2dPush(L, &v1);
        return 1;
    }

    if ((v2Ptr = vector2dPtrPop(L, 2)) != NULL)
    {
        v1 = v1+*v2Ptr;
        vector2dPush(L, &v1);
        return 1;
    }
    return 0;
}


int vectorSub(lua_State *L)
{
    Vec3d v1, *v2Ptr = NULL;

    if (!vectorPop(L, &v1, 1))
        return 0;

    if (lua_isnumber(L, 2))
    {
        v1 += (float)-lua_tonumber(L, -1);
        vectorPush(L, &v1);
        return 1;
    } else if ((v2Ptr = vectorPtrPop(L, 2)) != NULL)
    {
        v1 = v1-*v2Ptr;
        vectorPush(L, &v1);
        return 1;
    }
    return 0;
}

int vector2dSub(lua_State *L)
{
    Vec2d v1, *v2Ptr = NULL;

    if (!vector2dPop(L, &v1, 1))
        return 0;

    if (lua_isnumber(L, 2))
    {
        v1 += (float)-lua_tonumber(L, -1);
        vector2dPush(L, &v1);
        return 1;
    } else if ((v2Ptr = vector2dPtrPop(L, 2)) != NULL)
    {
        v1 = v1-*v2Ptr;
        vector2dPush(L, &v1);
        return 1;
    }
    return 0;
}

int vectorMul(lua_State *L)
{
    Vec3d v1, *v2Ptr = NULL;

    if (!vectorPop(L, &v1, 1))
        return 0;
    
    if (lua_isnumber(L, 2))
    {
        v1 = v1*((float)lua_tonumber(L, -1));
        vectorPush(L, &v1);
        return 1;
    } else if ((v2Ptr = vectorPtrPop(L, 2)) != NULL)
    {
        lua_pushnumber(L, v1.dot(*v2Ptr));
        return 1;
    }
    return 0;
}


int vector2dMul(lua_State *L)
{
    Vec2d v1, *v2Ptr = NULL;

    if (!vector2dPop(L, &v1, 1))
        return 0;

    if (lua_isnumber(L, 2))
    {
        v1 = v1*((float)lua_tonumber(L, -1));
        vector2dPush(L, &v1);
        return 1;
    } else if ((v2Ptr = vector2dPtrPop(L, 2)) != NULL)
    {
        lua_pushnumber(L, v1.dot(*v2Ptr));
        return 1;
    }
    return 0;
}

int vectorPow(lua_State *L)
{
    Vec3d v1, *v2Ptr = vectorPtrPop(L, 2);

    if (!vectorPop(L, &v1, 1) || !v2Ptr)
        return 0;
    
    v1 = v1^(*v2Ptr);
    vectorPush(L, &v1);
    return 1;
}

int vector2dPow(lua_State *L)
{
    Vec2d v1, *v2Ptr = vector2dPtrPop(L, 2);

    if (!vector2dPop(L, &v1, 1) || !v2Ptr)
        return 0;

    //v1 = v1^(*v2Ptr);
    vector2dPush(L, &v1);
    return 1;
}

int vectorSqrNorm(lua_State *L)
{
    Vec3d *v1Ptr = vectorPtrPop(L, 1);
    if (!v1Ptr)    return 0;
    lua_pushnumber(L, v1Ptr->sqrnorm());
    return 1;
}

int vector2dSqrNorm(lua_State *L)
{
    Vec2d *v1Ptr = vector2dPtrPop(L, 1);

    if (!v1Ptr)
        return 0;

    lua_pushnumber(L, v1Ptr->sqrnorm());
    return 1;
}

int vectorNorm(lua_State *L)
{
    Vec3d *v1Ptr = vectorPtrPop(L, 1);

    if (!v1Ptr)
        return 0;

    lua_pushnumber(L, v1Ptr->norm());
    return 1;
}

int vector2dNorm(lua_State *L)
{
    Vec2d *v1Ptr = vector2dPtrPop(L, 1);
    if (!v1Ptr)    return 0;
    lua_pushnumber(L, v1Ptr->norm());
    return 1;
}

int vectorNormalize(lua_State *L)
{
    Vec3d *v1Ptr = vectorPtrPop(L, 1);

    if (!v1Ptr)
        return 0;

    if (lua_gettop(L) > 1 && lua_toboolean(L, 2))
    {
        Vec3d v2 = *v1Ptr;
        v2.normalize();
        vectorPush(L, &v2);
    } else
    {
        v1Ptr->normalize();
        vectorPtrPush(L, v1Ptr);
    }
    return 1;
}

int vector2dNormalize(lua_State *L)
{
    Vec2d *v1Ptr = vector2dPtrPop(L, 1);

    if (!v1Ptr) return 0;

    if (lua_gettop(L) > 1 && lua_toboolean(L, 2))
    {
        Vec2d v2 = *v1Ptr;
        v2.normalize();
        vector2dPush(L, &v2);
    } else
    {
        v1Ptr->normalize();
        vector2dPtrPush(L, v1Ptr);
    }
    return 1;
}
static const luaL_reg vectormeta[] = {
    {"__index",        vectorIndex},
    {"__newindex",    vectorNewIndex},
    {"__tostring",    vectorToString},
    {"__add",        vectorAdd},
    {"__sub",        vectorSub},
    {"__mul",        vectorMul},
    {"__pow",        vectorPow},
    {0, 0}
};

static const luaL_reg vector2dmeta[] = {
    {"__index",        vector2dIndex},
    {"__newindex",    vector2dNewIndex},
    {"__tostring",    vector2dToString},
    {"__add",        vector2dAdd},
    {"__sub",        vector2dSub},
    {"__mul",        vector2dMul},
    {"__pow",        vector2dPow},
    {0, 0}
};

void vectorRegister(lua_State *L)
{
    luaL_newmetatable(L, "vector");                    /* create metatable for Foo,
                                                        and add it to the Lua registry */    
    luaL_openlib(L, 0, vectormeta, 0);                /* fill metatable */
    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, -3);                            /* dup methods table*/
    lua_rawset(L, -3);                                /* hide metatable:
                                                        metatable.__metatable = methods */
    lua_pop(L, 1);                                    /* drop metatable */
}
void vector2dRegister(lua_State *L)
{
    luaL_newmetatable(L, "vector2d");                    /* create metatable for Foo,
                                                       and add it to the Lua registry */    
    luaL_openlib(L, 0, vector2dmeta, 0);                /* fill metatable */
    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, -3);                            /* dup methods table*/
    lua_rawset(L, -3);                                /* hide metatable:
                                                      metatable.__metatable = methods */
    lua_pop(L, 1);                                    /* drop metatable */
}

void vectorPtrRegister(lua_State *L)
{
    luaL_newmetatable(L, "vectorPtr");                /* create metatable for Foo,
                                                        and add it to the Lua registry */
    luaL_openlib(L, 0, vectormeta, 0);                /* fill metatable */
    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, -3);                            /* dup methods table*/
    lua_rawset(L, -3);                                /* hide metatable:
                                                        metatable.__metatable = methods */
    lua_pop(L, 1);                                    /* drop metatable */
}

void vector2dPtrRegister(lua_State *L)
{
    luaL_newmetatable(L, "vector2dPtr");                /* create metatable for Foo,
                                                      and add it to the Lua registry */
    luaL_openlib(L, 0, vector2dmeta, 0);                /* fill metatable */
    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, -3);                            /* dup methods table*/
    lua_rawset(L, -3);                                /* hide metatable:
                                                      metatable.__metatable = methods */
    lua_pop(L, 1);                                    /* drop metatable */
}

int vectorNew(lua_State *L)
{
    Vec3d vect(0.0f, 0.0f, 0.0f);
    
    if (lua_gettop(L) == 3)
    {
        vect.m_x = (float)lua_tonumber(L, 1);
        vect.m_y = (float)lua_tonumber(L, 2);
        vect.m_z = (float)lua_tonumber(L, 3);
    }
    vectorPush(L, &vect);
    return 1;
}

int vector2dNew(lua_State *L)
{
    Vec2d vect(0.0f, 0.0f);

    if (lua_gettop(L) == 2)
    {
        vect.m_x = (float)lua_tonumber(L, 1);
        vect.m_y = (float)lua_tonumber(L, 2);
    }
    vector2dPush(L, &vect);
    return 1;
}
}