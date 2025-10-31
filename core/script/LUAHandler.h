#ifndef _ITF_LUAHANDLER_H_
#define _ITF_LUAHANDLER_H_

#ifndef _ITF_VEC3D_H_
#include "core/math/vec3d.h"
#endif //_ITF_VEC3D_H_

#include "core/system/Synchronize.h"

#ifndef _ITF_VEC2D_H_
#include "core/math/vec2d.h"
#endif //_ITF_VEC2D_H_

#define CAMCAM_COMPATIBILITY

struct lua_State;

#define REGISTER(_funcname, _funcimpl) lua_register(L, _funcname, _funcimpl); m_registeredFunctions.push_back(_funcname);

namespace ITF
{

	class Blob;
	class Lua;
    class String8;

    class LUAHandler
    {
    public:
        typedef struct LUAClients
        {
            String8               m_fileName;
            ITF_VECTOR<ObjectRef> m_clients;
        } LUAClients;
    private:
         ITF_VECTOR<LUAClients> m_LUAFilesClients;



        lua_State*   L;
        void* m_currentInitActor; 
        void registerBindedFunctions();

    public:
        ITF_VECTOR<const char*> m_registeredFunctions;
        int         m_isPaused;

        LUAHandler();
        ~LUAHandler();
        void	init();
	    void	shutdown();
        void    update                  ( float _dt  );
        void    pause();
        void	sleep(float _duration);
        void    registerBindedFunctions_Physics();
        void    registerBindedFunctions_Core();
        void    registerBindedFunctions_NETAdapter();
        void    registerBindedFunctions_Math();
        void    registerBindedFunctions_Gfx();
        void    registerBindedFunctions_Sound();
        void    registerBindedFunctions_GamePlay();
	    	    
        ITF_INLINE lua_State * getLuaState() { return L;}
        ITF_INLINE void* getCurrentInitActor() {return m_currentInitActor;}
        void           registerLUAFileUsage(const ObjectRef& _user, const String8& _fileName);
        void           unregisterLUAFileUsage(const ObjectRef& _user, const String8& _fileName);

        ITF_VECTOR<ObjectRef>* getLUAFileClients(const String8& _fileName);
        const ITF_VECTOR<LUAClients>* getFilesClients() {return &m_LUAFilesClients;}

        static u32     getU32FromTable(lua_State *L, const char * _name, u32 _default = 0, bbool* _success = NULL );
        static f32     getF32FromTable(lua_State *L, const char * _name, f32 _default = 0.f, bbool* _success = NULL );
        static void    getStringFromTable(lua_State *L, const char * _name, String & _result, const String& _default = String("") );
        static void    getString8FromTable(lua_State *L, const char * _name, String8 & _result, const String8& _default = String8("") );
        static const char * getCharFromTable(lua_State *L, const char * _name, const char * _default = NULL);
        static StringID getStringIDFromTable(lua_State *L, const char* _name, const StringID& _default = StringID::Invalid );
        static Vec3d   getVec3dFromTable(lua_State *L, const char * _name, const Vec3d& _default = Vec3d::Zero, bbool* _success = NULL );
        static Vec2d   getVec2dFromTable(lua_State *L, const char * _name, const Vec2d& _default = Vec2d::Zero, bbool* _success = NULL );
        static void    getVec2dListFromTable(lua_State *L, const char * _name, SafeArray <Vec2d>& _list, bbool* _success = NULL );
        static u32     getU32FromTable(lua_State *L, u32 _index);
        static f32     getF32FromTable(lua_State *L, u32 _index);
        static void    getStringFromTable(lua_State *L, u32 _index, String & _result);
        static void    getString8FromTable(lua_State *L, u32 _index, String8 & _result);
        static const char * getCharFromTable(lua_State *L, u32 _index,const char * _default = NULL);
        static Vec3d   getVec3dFromTable(lua_State *L, u32 _index);
        static Vec2d   getVec2dFromTable(lua_State *L, u32 _index);
        
        static bool    openGlobalTable(lua_State * L, const char * _name);
        static bool    openGlobalTable(lua_State * L, const char * _name, i32 & _tableSize);
        static void    closeGlobalTable(lua_State * L);

        static bool    openTable(lua_State * L, const char * _name, i32 & _tableSize);
        static bool    openTable(lua_State * L, const char * _name);
        static bool    openTable(lua_State * L, u32 _index, i32 & _tableSize);
        static bool    openTable(lua_State * L, u32 _index);
        static void    closeTable(lua_State * L);
        static void    getStateInfo(lua_State *L, String8& _dest);

        //void    setGlobalContext(void * _value);
        //void *  getGlobalContext();

		//Begin Thread Safe function
		void	executeFile(const char* _fileName);
		void	executeString(const char* _command);
        void    executeActorInit(const String8 & _functionName, void * _context);
        bbool   isSafe()        {return m_IsSafe;}
		ITF_THREAD_CRITICAL_SECTION& getCS() {return m_csLua;}
		//End Thread Safe function
        ITF_INLINE const ITF_VECTOR<ObjectRef>& getResources() {return m_luaResources;}

        void printError(lua_State * L, int s, const char * _function, bbool _raise = btrue);

        void  cleanError()  {m_HasError = bfalse;}
        bbool hasError()    {return m_HasError;}

    private:
        ITF_VECTOR<ObjectRef> m_luaResources;

        bbool                 m_IsSafe;
        bbool                 m_HasError;

		ITF_THREAD_CRITICAL_SECTION             m_csLua;

    };



} // namespace ITF


#endif // _ITF_LUAHANDLER_H_
