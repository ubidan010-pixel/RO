#include "precompiled_engine.h"

#ifdef ITF_SUPPORT_LOGICDATABASE

#ifndef _ITF_LOGICDATABASE_H_
#include "engine/logicdatabase/logicdatabase.h"
#endif //_ITF_LOGICDATABASE_H_

#ifndef _ITF_LUABINDEDFUNCTIONS_MATH_H_
#include "core/script/LuaBinderFunctions_Math.h"
#endif //_ITF_LUABINDEDFUNCTIONS_MATH_H_

//#define ENABLE_LOG
#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_FILECACHEMANAGER_H_
#include "core/AdaptersInterfaces/FileCacheManager.h"
#endif //_ITF_FILECACHEMANAGER_H_


extern "C" 
{
#include "lua/src/lua.h"
#include "lua/src/lualib.h"
#include "lua/src/lauxlib.h"
}

namespace ITF
{
#ifdef ITF_SUPPORT_LUA

    void  fillLogicDataBase(lua_State *L,LogicDataTable* _pLogicTableRoot, LogicDataTableDependenciesCollector*  _collector)
    {
        LogicDataTable * pRoot = _pLogicTableRoot;

        lua_pushnil(L);
        while (lua_next(L, -2))
        {
            u32 index = U32_INVALID;
            i32 clefType = lua_type(L, -2); // -> clef type "number" / "string"
            int typeValeur = lua_type(L, -1); // -> value type "number" /string/table

            lua_pushvalue(L,-2);
            const char * key = lua_tostring(L, -1); // key name
            lua_pop(L,1);

            LogicDataProperty * dataProperty = NULL;

            if (clefType == LUA_TNUMBER)
            {
                lua_pushvalue(L,-2);
                index = u32(lua_tointeger(L, -1))-1;
                lua_pop(L,1);
            }

            if (typeValeur == LUA_TNUMBER)
            {
                f32 value = (f32)lua_tonumber(L, -1);
                dataProperty = pRoot->add(key,value);
                if (_collector)
                {
                    String lowerKey = key;

                    lowerKey.toLower();
                    _collector->m_numValues[lowerKey] = value;
                }
            }
            else if (typeValeur == LUA_TSTRING)
            {
                const char* stringValue = lua_tostring(L, -1);
                dataProperty = pRoot->add(key,stringValue);

                if (_collector)
                {
                    String slower = stringValue;
                    String skey   = key;

                    slower.toLower();
                    skey.toLower();
                    _collector->m_strValues[skey] = slower;
                }

            }
            else if (typeValeur == LUA_TUSERDATA)
            {
                void    *p        = lua_touserdata(L, -1);
                ITF_ASSERT(p);

                if (lua_getmetatable(L, -1))
                {

                    lua_getfield(L, LUA_REGISTRYINDEX, "vector2d");
                    if (lua_rawequal(L, -1, -2))
                    {
                        Vec2d * vect2d= (Vec2d *) p;
                        dataProperty = pRoot->add(eLogicDataVector2d,key,*vect2d);
                    }

                    lua_pop(L, 1);

                    lua_getfield(L, LUA_REGISTRYINDEX, "vector");
                    if (lua_rawequal(L, -1, -2))
                    {
                        Vec3d * vect3d= (Vec3d *) p;
                        dataProperty = pRoot->add(eLogicDataVector3d,key,*vect3d);
                    }

                    lua_pop(L, 1);


                }

                lua_pop(L, 1);
            }
            else if (typeValeur == LUA_TTABLE)
            {
                u32  tableSize = u32(lua_objlen(L,-1));

                LogicDataTable* nodecacheTable = newAlloc(mId_LogicDataBase,LogicDataTable());
                nodecacheTable->setCount(tableSize);

                LogicDataTableDependenciesCollector* child = NULL;

                if (_collector)
                {
                    child = new LogicDataTableDependenciesCollector;
                    _collector->m_childDep.push_back( child );
                }
                fillLogicDataBase(L,nodecacheTable,child);

                nodecacheTable->shrink();

                dataProperty = pRoot->add(key,nodecacheTable);

            }
            if (index != U32_INVALID && dataProperty)
            {
                pRoot->setIndex(index,dataProperty);

            }

            lua_pop(L, 1);
        }
    }

    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    int LogicDataBase::includeReference(lua_State *L)
    {    
#ifdef ASSERT_ENABLED
        int argc = 
#endif
            lua_gettop(L);
        ITF_ASSERT(argc == 1);
        const char * fileName = lua_tostring(L,1);

        File* pFile = FILECACHEMANAGER->openFile(fileName);
        if (pFile)
        {
            u32 fileSize = (u32)pFile->getLength();
            char * readBuffer = newAlloc(mId_Temporary, char[fileSize+1]);
            u32 sizeRead = 0;
            pFile->read(readBuffer,fileSize,&sizeRead);
            ITF_ASSERT(fileSize == sizeRead);
            readBuffer[fileSize] = '\0';
            FILECACHEMANAGER->closeFile(pFile);
            pFile = NULL;
            int s = luaL_loadstring(L, readBuffer);

            lua_getglobal(L,"dependencyOwner");

            LogicDataTableDependencies* pLogicDataDepency = (LogicDataTableDependencies*) lua_topointer(L,-1);
            if (pLogicDataDepency)
            {
                pLogicDataDepency->addDependency(fileName);
            }


            lua_pop(L,1);

            if ( s==0 ) 
            {
                s = lua_pcall(L, 0, LUA_MULTRET, 0);
                if (s != 0)
                {
                    m_luaHandler->printError(L,s,readBuffer,m_bRaiseError);
                }
            }
            else
            {
                m_luaHandler->printError(L,s,readBuffer,m_bRaiseError);
            }
            SF_DEL_ARRAY(readBuffer);
        }
        else
        {
            String msg;
            msg.setTextFormat("Cannot include file: \"%s\"\nFrom file: \"%ls\"", fileName, LOGICDATABASE->getCurrentRefName().cStr());
#if defined(ITF_WINDOWS)
            if(SYSTEM_ADAPTER->IsSilent() && PLUGINGATEWAY)
            {
                PLUGINGATEWAY->OnRefMissing(fileName,LOGICDATABASE->getCurrentRefName());
            }
            else
            {
                switch(SYSTEM_ADAPTER->messageBox("Data error", msg + "\n\nDo you want to open the incriminated file's folder ?", ITF_MSG_YESNO,ITF_IDNO))
                {
                case ITF_IDYES:
                    {
                        SystemAdapter::findInExplorer(LOGICDATABASE->getCurrentRefName());
                    }
                    break;
                }
            }
#else // !ITF_WINDOWS
            ITF_ERROR("%ls", msg.cStr());
#endif // ITF_WINDOWS
        }
        return 0;
    }

    //parse the file and fill the appropriate cache 
    LogicDataLink* LogicDataBase::createFileByBuffer(const String& _relativePath,const char * readonlyBuffer)
    {
        String keyname = _relativePath;
        keyname.toLower();

        LUAHandler luaHandler;
        LogicDataTable* nodecacheTable = newAlloc(mId_LogicDataBase,LogicDataTable());

        lua_State* L = luaHandler.getLuaState();

        luaHandler.executeString("params={}");
        luaHandler.executeString(readonlyBuffer);

        {
            lua_getglobal(L,"params");
            fillLogicDataBase(L,nodecacheTable,NULL);
        }

        ArchiveMemory aw;
        saveIdToFile(keyname,"PC",nodecacheTable,aw,true);

        ArchiveMemory rd(aw.getData(),aw.getSize());
        LogicDataLink* pLogicDataLink = readFromBuffer(rd);

        {
            csAutoLock cs(m_cs);
            m_LogicDataTables[keyname]   = pLogicDataLink;
        }

        nodecacheTable->destroy();
        SF_DEL(nodecacheTable);


        return pLogicDataLink;
    }


    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    int includeReferenceProxy(lua_State *L)
    {
        return LOGICDATABASE->includeReference(L);
    }

    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    CompilLogicDataInfo LogicDataBase::compileBuffer(const String& _vPlatform,const String& _relativePath,char* readBuffer,LogicDataTableDependenciesCollector* _CollectorDependencies)
    {
        m_currentRefName = _relativePath;
        m_luaHandler = new LUAHandler;

        CompilLogicDataInfo compilInfo;
        compilInfo.nodecacheTable = newAlloc(mId_LogicDataBase,LogicDataTable());
        compilInfo.dependency = newAlloc(mId_LogicDataBase,LogicDataTableDependencies());
        compilInfo.dependency->addDependency(_relativePath);

        lua_State* L = m_luaHandler->getLuaState();
        lua_register(L, "includeReference", includeReferenceProxy);
        lua_pushlightuserdata(L,(void*)compilInfo.dependency);
        lua_setglobal(L,"dependencyOwner");

        StringConverter vPlatform8( _vPlatform.cStr());
        lua_pushstring(L, vPlatform8.getChar());
        lua_setglobal(L,"ITF_Platform");

#ifndef ITF_FINAL
        if (SYSTEM_ADAPTER->hasDebugInput())
        {
            lua_pushstring(L, SYSTEM_ADAPTER->getDebugInput());
            lua_setglobal(L, "ITF_DebugInput");
        }
#endif

        m_luaHandler->executeString("params={}");

        //TODO remove this hardcoded path
        m_luaHandler->executeString("includeReference(\"Actor/Includes/helpers.ilu\")");
        m_luaHandler->executeString("includeReference(\"Actor/Includes/gameplay_types.ilu\")");
        m_luaHandler->executeString(readBuffer);

        SF_DEL_ARRAY(readBuffer);

        m_luaHandler->executeString("ITF_cleanActor()");

        {
            lua_getglobal(L,"params");
            fillLogicDataBase(L,compilInfo.nodecacheTable,_CollectorDependencies);
        }

        return compilInfo;
    }

#endif //ITF_SUPPORT_LUA


} // namespace ITF

#endif //ITF_SUPPORT_LOGICDATABASE
