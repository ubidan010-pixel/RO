#include "precompiled_core.h"

#ifdef ITF_SUPPORT_LUA

#ifndef _ITF_LUAHANDLER_H_
#include "core/script/LUAHandler.h"
#endif //_ITF_LUAHANDLER_H_

#ifndef _ITF_LUABINDEDFUNCTIONS_MATH_H_
#include "core/script/LuaBinderFunctions_Math.h"
#endif //_ITF_LUABINDEDFUNCTIONS_MATH_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_BLOB_H_
#include "core/Blob.h"
#endif //_ITF_BLOB_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

extern "C" 
{
#include "lua/src/lua.h"
#include "lua/src/lualib.h"
#include "lua/src/lauxlib.h"
}

static void *LUA_alloc (void *ud, void *ptr, size_t osize, size_t nsize) {
  (void)ud;
  (void)osize;
  if (nsize == 0) {  

	  if (ptr)
		ITF::Memory::free(ptr);
    return NULL;
  }
  else
  {
  	if(!ptr) // check may be done by realloc function, but if not ...
        return ITF::Memory::mallocCategory(nsize, ITF::MemoryId::mId_System);
  	else
        return ITF::Memory::realloc(ptr, nsize);		
  }
}


static int LUA_panic (lua_State *L) {
  (void)L;  /* to avoid warnings */
  ITF_ERROR("LUA PANIC: unprotected error in call to Lua API (%s)\n",
                   lua_tostring(L, -1));
  return 0;
}

namespace ITF
{
#define   IS_SAFE()  ITF_ASSERT(m_IsSafe)

    class    csAutoLockLua :public csAutoLock
    {
    public:

        ITF_INLINE csAutoLockLua(ITF_THREAD_CRITICAL_SECTION &tcs,bbool* _safe) :csAutoLock(tcs)
         {
              *_safe = btrue;   
              _val   = _safe;
          };

        ITF_INLINE virtual ~csAutoLockLua()
        {
            *_val = bfalse;   
        }

    protected:
        bbool *_val;
    };

    int FillScriptInfo(lua_State *L)
    {
        lua_Debug   ar[64];
        int               nbLevels = 1;
        char megaBuffer[4096];
        char        *inScriptErrorPtr = megaBuffer;
        lua_getstack(L, 0, &ar[0]);
        lua_getinfo(L, "nlS", &ar[0]);
        for (; nbLevels < 64 && lua_getstack(L, nbLevels, &ar[nbLevels]); nbLevels++)
            lua_getinfo(L, "nlS", &ar[nbLevels]);
        for (nbLevels--; nbLevels > 1; nbLevels--)
        {
            sprintf(inScriptErrorPtr, "%s Line %i\n", ar[nbLevels].name, ar[nbLevels].currentline);
            inScriptErrorPtr += strlen(inScriptErrorPtr);
        }

        //if (isError)
        //    sprintf(inScriptErrorPtr, "%s:%i: bad argument #%i to '%s\'", ar[1].short_src, ar[1].currentline, paramNum, ar[0].name);
        //else
        sprintf(inScriptErrorPtr, "%s:%i: function '%s\'", ar[1].name, ar[1].currentline, ar[0].name);
        lua_pushstring(L,megaBuffer);
        return 1;
    }


    void LUAHandler::printError(lua_State * LState, int s, const char * _function, bbool _raise)
    {
        char * luaRes = (char *)lua_tostring(LState, -1);
        switch (s)
        {
        case LUA_ERRMEM:
            ITF_ERROR_SHOW(_raise, "Lua memory allocation error, %s in %s ",luaRes,_function);
            break;
        case LUA_ERRSYNTAX:
            ITF_ERROR_SHOW(_raise, "Lua syntax error, %s in %s",luaRes,_function);
            break;
        default:
            ITF_ERROR_SHOW(_raise, "Lua error, %s in %s",luaRes,_function);
            break;
        }
        lua_pop(LState, 1);
        m_HasError = btrue;
    }


    i32 push(lua_State *luaState,f32 _value) 
    {
        lua_pushnumber(luaState,_value);
        return 1;
    }
    i32 push(lua_State *luaState,f64 _value) 
    {
        lua_pushnumber(luaState,_value);
        return 1;
    }
    i32 push(lua_State *luaState,u32 _value) 
    {
        lua_pushinteger(luaState,_value);
        return 1;
    }
    i32 push(lua_State *luaState,i32 _value) 
    {
        lua_pushinteger(luaState,_value);
        return 1;
    }
    i32 push(lua_State *luaState,const char * _value)
    {
        lua_pushstring(luaState,_value);
        return 1;
    }

    void pushInTable(lua_State * luaState,f32 _value, int & _tableIndex) 
    {
        lua_pushnumber(luaState,_tableIndex);
        push(luaState,_value);
        lua_settable(luaState,-3);
        ++_tableIndex;
    }
    void pushInTable(lua_State * luaState,f64 _value, int & _tableIndex) 
    {
        lua_pushnumber(luaState,_tableIndex);
        push(luaState,_value);
        lua_settable(luaState,-3);
        ++_tableIndex;
    }
    void pushInTable(lua_State * luaState,u32 _value, int & _tableIndex) 
    {
        lua_pushinteger(luaState,_tableIndex);
        push(luaState,_value);
        lua_settable(luaState,-3);
        ++_tableIndex;
    }
    void pushInTable(lua_State * luaState,i32 _value, int & _tableIndex) 
    {
        lua_pushinteger(luaState,_tableIndex);
        push(luaState,_value);
        lua_settable(luaState,-3);
        ++_tableIndex;
    }
    void pushInTable(lua_State * luaState,const char* _value, int & _tableIndex) 
    {
        lua_pushinteger(luaState,_tableIndex);
        push(luaState,_value);
        lua_settable(luaState,-3);
        ++_tableIndex;
    }

    void pushBlobInTable(lua_State * luaState, Blob * _blob)
    {
        ITF_ASSERT(luaState);
        ITF_ASSERT(_blob);

        _blob->makeReadOnly();

        int tableIndex = 1;
        u32 numBytesRemaining = _blob->getNumberOfRemainingBytes();

        while(numBytesRemaining > 0)
        {
            const u32 seekPos = _blob->getSeekPos();
            DataType type = _blob->extractBlobDataType(0);
            _blob->setSeekPos(seekPos);

            switch(type)
            {
            case DataType_f32:
                pushInTable(luaState,_blob->extractFloat32(),tableIndex);
                break;
            case DataType_f64:
                pushInTable(luaState,_blob->extractFloat64(),tableIndex);
                break;
            case DataType_i32:
                pushInTable(luaState,_blob->extractInt32(),tableIndex);
                break;
            case DataType_u32:
                pushInTable(luaState,_blob->extractUint32(),tableIndex);
                break;
            case DataType_String:
                {
                    String strg;
                    _blob->extractString(strg);
                    pushInTable(luaState,StringConverter(strg).getChar(),tableIndex);
                }
                break;
            default:
                LOG("Unsupported Data type %x",type);
                ITF_ASSERT(0);
            }
            numBytesRemaining = _blob->getNumberOfRemainingBytes();
        }
    }



LUAHandler::LUAHandler()
: L(NULL)
, m_currentInitActor(NULL)

{
    init();
}

LUAHandler::~LUAHandler()
{
    shutdown();
}

void LUAHandler::init()
{
    ITF_ASSERT_MSG(!L, "Already initiated !");

	L = lua_newstate(LUA_alloc, NULL);
	if(L)
		lua_atpanic(L, &LUA_panic);

    m_IsSafe = bfalse;
    m_HasError = bfalse;

    luaopen_base(L);
    luaopen_table(L);
    luaopen_string(L);
    luaopen_math(L);

    registerBindedFunctions();
    m_isPaused = 1;

    lua_checkstack(L,100);

    Synchronize::createCriticalSection(&m_csLua);
}

void LUAHandler::shutdown()
{
    ITF_ASSERT(m_LUAFilesClients.size()==0);

    lua_close(L);
    m_registeredFunctions.clear();

    Synchronize::destroyCriticalSection(&m_csLua);
}

void LUAHandler::sleep(float _time)
{
   /* m_sleepTimer = _time;
    pause();*/
}

void LUAHandler::pause()
{
    if (!m_isPaused)
    {
        m_isPaused = 1;
    }
}

void LUAHandler::update( float _dt )
{
}


void LUAHandler::registerBindedFunctions()
{
    registerBindedFunctions_Math();
}

void LUAHandler::executeFile(const char* _fileName)
{
    csAutoLockLua cs(m_csLua,&m_IsSafe);
    File* f = FILEMANAGER->openFile(_fileName,ITF_FILE_ATTR_READ);
    if (f)
    {
        u32 fileSize = (u32)f->getLength();
        char * readBuffer = newAlloc(mId_Temporary,char[fileSize+1]);
        u32 sizeRead = 0;
        f->read(readBuffer,fileSize,&sizeRead);
        ITF_ASSERT(fileSize == sizeRead);
        readBuffer[fileSize] = '\0';
        FILEMANAGER->closeFile(f);

        executeString(readBuffer);
        SF_DEL_ARRAY(readBuffer);
    }
    else
    {
        ITF_ASSERT(0);
    }

    LOG("LUA executing file %s\n",_fileName);
}


void LUAHandler::executeString(const char* _command)
{
    csAutoLockLua cs(m_csLua,&m_IsSafe);

    lua_pushcfunction(L,FillScriptInfo);
    int pos = lua_gettop(L);

    int s = luaL_loadstring(L, _command);

    String8 commandTruncated = _command;
    commandTruncated.truncate(512);

    if ( s==0 ) 
    {
      s = lua_pcall(L, 0, LUA_MULTRET, pos);
      if (s != 0)
      {
          printError(L,s, commandTruncated.cStr());
      }
    }
    else
    {
        printError(L,s,commandTruncated.cStr());
    }
}


//void LUAHandler::setGlobalContext(void * _value)
//{
//    lua_getglobal(L,"ITF");
//    int i = lua_gettop(L);
//    lua_pushstring(L,"currentContext");
//    lua_pushlightuserdata(L,_value);
//    lua_settable(L,-3);
//    lua_pop(L,1);
//}
//
//void * LUAHandler::getGlobalContext()
//{
//    lua_getglobal(L,"ITF");
//    int i = lua_gettop(L);
//    lua_pushstring(L,"currentContext");
//    lua_gettable(L,-2);
//    void * result = lua_topointer(L,-1);
//    lua_pop(L,2);
//    return result;
//}

void LUAHandler::executeActorInit(const String8 & _functionName, void * _actor)
{
    csAutoLockLua cs(m_csLua,&m_IsSafe);
    m_currentInitActor = _actor;

    //get function
    lua_getglobal(L,_functionName.cStr());

    //push context
    lua_pushlightuserdata(L,_actor);

    //call function
    int s = lua_pcall(L, 1, 0, 0);
    if (s != 0)
    {
        printError(L,s,_functionName.cStr(),false);
    }
      
}

u32 LUAHandler::getU32FromTable(lua_State *L, const char * _name, u32 _default, bbool* _success )
{
    lua_pushstring(L,_name);
    lua_gettable(L,-2);

    u32 result = _default;

    if (lua_isnumber(L,-1))
    {
        if(_success)
            *_success = btrue;
        result = (u32)lua_tonumber(L,-1);
    }
    else
    {
        if(_success)
            *_success = bfalse;        
    }

    lua_pop(L,1);
    return result;
}

f32 LUAHandler::getF32FromTable(lua_State *L, const char * _name, f32 _default, bbool* _success )
{
    lua_pushstring(L,_name);
    lua_gettable(L,-2);

    f32 result = _default;

    if (lua_isnumber(L,-1))
    {
        if(_success)
            *_success = btrue;
        result = (f32)lua_tonumber(L,-1);
    }
    else
    {        
        if(_success)
            *_success = bfalse;
    }

    lua_pop(L,1);

    return result;
}

void LUAHandler::getStringFromTable(lua_State *L, const char * _name, String & _result, const String& _default )
{
    lua_pushstring(L,_name);
    lua_gettable(L,-2);

    if (lua_isstring(L,-1))
    {
        _result = lua_tostring(L,-1);
    }
    else
    {
        _result = _default;
    }

    lua_pop(L,1);
}

void LUAHandler::getString8FromTable(lua_State *L, const char * _name, String8& _result, const String8& _default )
{
    lua_pushstring(L,_name);
    lua_gettable(L,-2);

    if (lua_isstring(L,-1))
    {
        _result = lua_tostring(L,-1);
    }
    else
    {
        _result = _default;
    }

    lua_pop(L,1);
}

const char * LUAHandler::getCharFromTable(lua_State *L, const char * _name,const char *  _default )
{
    lua_pushstring(L,_name);
    lua_gettable(L,-2);
    if (lua_isstring(L,-1))
    {
        const char * result = lua_tostring(L,-1);
        lua_pop(L,1);
        return result;
        
    }
    else
    {
        lua_pop(L,1);
        return _default;
    }
}

StringID LUAHandler::getStringIDFromTable(lua_State *L, const char* _name, const StringID& _default )
{
    const char* str = getCharFromTable(L,_name,NULL);

    if ( str )
    {
        return StringID(str);
    }
    else
    {
        return _default;
    }
}

Vec3d LUAHandler::getVec3dFromTable(lua_State *L, const char * _name, const Vec3d& _default, bbool* _success )
{
    lua_pushstring(L,_name);
    lua_gettable(L,-2);
    Vec3d vec = _default;

    if ( !vectorPop(L,&vec,-1) )
    {
        if (_success)
            *_success = bfalse;        
    }
    else if (_success)
            *_success = btrue;

    lua_pop(L,1);
    return vec;
}

Vec2d LUAHandler::getVec2dFromTable(lua_State *L, const char * _name, const Vec2d& _default, bbool* _success )
{
    lua_pushstring(L,_name);
    lua_gettable(L,-2);
    Vec2d vec;

    if ( !vector2dPop(L,&vec,-1) )
    {
        vec = _default;
        if (_success)
            *_success = bfalse;
    }
    else if (_success)
        *_success = btrue;

    lua_pop(L,1);
    return vec;
}

void LUAHandler::getVec2dListFromTable( lua_State *L, const char * _name, SafeArray <Vec2d>& _list, bbool* _success )
{
    i32 nbNum = 0;

    if (LUAHandler::openTable(L,_name,nbNum) && nbNum >= 1 )
    {
        _list.resize(nbNum);

        for ( i32 i = 0; i < nbNum; i++ )
        {
            _list[i] = LUAHandler::getVec2dFromTable(L,i+1);
        }

        LUAHandler::closeTable(L);
    }
}

u32 LUAHandler::getU32FromTable(lua_State *L, u32 _index)
{
    lua_pushinteger(L,_index);
    lua_gettable(L,-2);
    u32 result = (u32)lua_tonumber(L,-1);
    lua_pop(L,1);
    return result;
}

f32 LUAHandler::getF32FromTable(lua_State *L, u32 _index)
{
    lua_pushinteger(L,_index);
    lua_gettable(L,-2);
    f32 result = (f32)lua_tonumber(L,-1);
    lua_pop(L,1);
    return result;
}

void LUAHandler::getStringFromTable(lua_State *L, u32 _index, String & _result)
{
    lua_pushinteger(L,_index);
    lua_gettable(L,-2);
    _result = lua_tostring(L,-1);
    lua_pop(L,1);
}

void LUAHandler::getString8FromTable(lua_State *L, u32 _index, String8 & _result)
{
    lua_pushinteger(L,_index);
    lua_gettable(L,-2);
    _result = lua_tostring(L,-1);
    lua_pop(L,1);
}

const char * LUAHandler::getCharFromTable(lua_State *L, u32 _index,const char *  _default )
{
    lua_pushinteger(L,_index);
    lua_gettable(L,-2);
    const char * temp = _default;
    if (lua_isstring(L,-1))
    {
        temp = lua_tostring(L,-1);
    }
    lua_pop(L,1);
    return temp;
}

Vec3d LUAHandler::getVec3dFromTable(lua_State *L, u32 _index)
{
    lua_pushinteger(L,_index);
    lua_gettable(L,-2);
    Vec3d vec;
    vectorPop(L,&vec,-1);
    lua_pop(L,1);
    return vec;
}
Vec2d LUAHandler::getVec2dFromTable(lua_State *L, u32 _index)
{
    lua_pushinteger(L,_index);
    lua_gettable(L,-2);
    Vec2d vec;
    vector2dPop(L,&vec,-1);
    lua_pop(L,1);
    return vec;
}


bool LUAHandler::openGlobalTable(lua_State * L, const char * _name)
{
    i32 unused;
    return openGlobalTable(L, _name, unused);
}

bool LUAHandler::openGlobalTable(lua_State * L, const char * _name, i32 & _tableSize)
{
    lua_getglobal(L,_name);
    if (lua_istable(L,-1))
    {
        _tableSize = i32(lua_objlen(L,-1));
        return true;
    } else
    {
        _tableSize = 0;
        lua_pop(L,1);
        return false;
    }
}

void LUAHandler::closeGlobalTable(lua_State * L)
{
    lua_pop(L,1);
}

bool LUAHandler::openTable(lua_State * L, const char * _name)
{
    i32 unused;
    return openTable(L, _name, unused);
}


bool LUAHandler::openTable(lua_State * L, const char * _name, i32 & _tableSize)
{
    lua_pushstring(L,_name);
    lua_gettable(L,-2);
    if (lua_istable(L,-1))
    {
        _tableSize = i32(lua_objlen(L,-1));
        return true;
    }
    else
    {
        _tableSize = 0;
        lua_pop(L,1);
        return false;
    }

}
bool LUAHandler::openTable(lua_State * L, u32 _index)
{
    i32 unused;
    return openTable(L, _index, unused);
}

bool LUAHandler::openTable(lua_State * L, u32 _index, i32 & _tableSize)
{
    lua_pushinteger(L,_index);
    lua_gettable(L,-2);
    if (lua_istable(L,-1))
    {
        _tableSize = i32(lua_objlen(L,-1));
        return true;
    }
    else
    {
        _tableSize = 0;
        lua_pop(L,1);
        return false;
    }
}

void LUAHandler::closeTable(lua_State * L)
{
    lua_pop(L,1);
}



void LUAHandler::unregisterLUAFileUsage(const ObjectRef& _user, const String8& _fileName)
{
#ifdef ITF_WINDOWS

    ITF_VECTOR<LUAClients>::iterator it = m_LUAFilesClients.begin();
    ITF_VECTOR<LUAClients>::iterator end = m_LUAFilesClients.end();

    for (; it != end; ++it)
    {
        LUAClients& c = *it;
        if (c.m_fileName == _fileName)
        {
            ITF_VECTOR<ObjectRef>::const_iterator sit = c.m_clients.begin();
            
            while (sit!=c.m_clients.end())
            {
                if (*sit == _user)
                {
                    c.m_clients.erase(sit);//don't need to continue..

                    if (c.m_clients.size() == 0)
                    {
                        m_LUAFilesClients.erase(it);
                    }

                    return;
                }
                else
                    sit++;
            }

            return;
        }
    }

#endif // ITF_WINDOWS
}


void LUAHandler::registerLUAFileUsage(const ObjectRef& _user, const String8& _fileName)
{
#ifdef ITF_WINDOWS
    ITF_VECTOR<LUAClients>::iterator it = m_LUAFilesClients.begin();
    ITF_VECTOR<LUAClients>::iterator end = m_LUAFilesClients.end();

    for (; it != end; ++it)
    {
        LUAClients& c = *it;
        if (c.m_fileName == _fileName)
        {
            ITF_VECTOR<ObjectRef>::const_iterator sit = c.m_clients.begin();
            ITF_VECTOR<ObjectRef>::const_iterator send = c.m_clients.end();

            for (; sit != send; ++sit)
            {
                if (*sit == _user)
                    return;
            }
            c.m_clients.push_back(_user);
            return;
        }
    }
    LUAClients c;
    c.m_fileName = _fileName;
    c.m_fileName.toLower();
    c.m_clients.push_back(_user);
    m_LUAFilesClients.push_back(c);
#endif // ITF_WINDOWS
}

ITF_VECTOR<ObjectRef>* LUAHandler::getLUAFileClients(const String8& _fileName)
{
    ITF_VECTOR<LUAClients>::iterator it = m_LUAFilesClients.begin();
    ITF_VECTOR<LUAClients>::iterator end = m_LUAFilesClients.end();

    for (; it != end; ++it)
    {
        LUAClients& c = *it;
        if (c.m_fileName == _fileName)
        {
            return &c.m_clients;
        }
    }
    return NULL;
}

void LUAHandler::getStateInfo(lua_State *L, String8& _dest)
{
    lua_Debug   ar[64];
    int               nbLevels = 1;
    char megaBuffer[4096];
    char        *inScriptErrorPtr = megaBuffer;
    lua_getstack(L, 0, &ar[0]);
    lua_getinfo(L, "nlS", &ar[0]);
    for (; nbLevels < 64 && lua_getstack(L, nbLevels, &ar[nbLevels]); nbLevels++)
        lua_getinfo(L, "nlS", &ar[nbLevels]);
    for (nbLevels--; nbLevels > 1; nbLevels--)
    {
        sprintf(inScriptErrorPtr, "%s Line %i\n", ar[nbLevels].name, ar[nbLevels].currentline);
        inScriptErrorPtr += strlen(inScriptErrorPtr);
    }

    //if (isError)
    //    sprintf(inScriptErrorPtr, "%s:%i: bad argument #%i to '%s\'", ar[1].short_src, ar[1].currentline, paramNum, ar[0].name);
    //else
    sprintf(inScriptErrorPtr, "source:%s\nname:%s:%i: function '%s\'\n", ar[1].source, ar[1].name, ar[1].currentline, ar[0].name);
    _dest = megaBuffer;    
}

} //namespace ITF

#endif //ITF_USE_LUA
