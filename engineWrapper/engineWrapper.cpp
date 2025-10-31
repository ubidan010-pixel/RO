// engineInterface.cpp : Defines the exported functions for the DLL application.


// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "../PluginGateway/PluginGateway.h"
#endif // _ITF_PLUGINGATEWAY_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core\error\ErrorHandler.h"
#endif // _ITF_ERRORHANDLER_H_

#ifndef _ITF_SCENE_H_
#include "engine\scene\scene.h"
#endif // _ITF_SCENE_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif // _ITF_SYSTEMADAPTER_

#ifndef _ITF_EDITORCONTEXTMENU_H_
    #include "editor\EditorContextMenu.h" // avoids stupid warning
#endif

#ifndef _ITF_PICKABLE_H_
#include "engine/picking/Pickable.h"
#endif // _ITF_PICKABLE_H_

#ifndef _ITF_PICKINGSHAPE_H_
#include "editor/PickingShape.h"
#endif // _ITF_PICKINGSHAPE_H_

#ifndef _ITF_NETADAPTER_H_
#include "engine/AdaptersInterfaces/NetAdapter.h"
#endif //_ITF_NETADAPTER_H_

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#include "engine/profiler/profilerMacro.h"

#ifndef _ITF_ENGINESIGNATURE_H_
#include "engine/signature/engineSignature.h"
#endif //_ITF_ENGINESIGNATURE_H_

#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <crtdbg.h>

using namespace System;
using namespace System::IO;

#include "engineWrapper.h"

engineWrapper::engineWrapper(String^ name)
{/*
    HANDLE hLogFile = CreateFileA("c:\\dumpLeaks.txt", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_WARN , _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, hLogFile);*/
   // _CrtSetBreakAlloc(11465906);

    ITF::Singletons::init();
    m_bIsConnected = false;
    m_szExename = Path::GetFileNameWithoutExtension(name);
}

void engineWrapper::destroy()
{
	ITF::Singletons::get().destroy();
}

void engineWrapper::disconnect()
{
    if (ITF::PluginGateWay::getSingleton() && m_bIsConnected)
    {
        ITF::PluginGateWay::destroySingleton();
        m_bIsConnected = false;
    }
}


void engineWrapper::putThisThreadAsMain()
{
    //all cook operation are done in this thread
    ITF::ThreadSettings::m_settings[ITF::eThreadId_mainThread].m_threadID = ITF::Synchronize::getCurrentThreadId();
}

//------------------------------------------------------------------------------------------------------------------------------------------------

bool engineWrapper::connectToHost(String^ address,int port)
{
    char* myAddress=utils::getCharCopy(address);

    if (!ITF::PluginGateWay::getSingleton())
        ITF::PluginGateWay::createSingleton();

    m_bIsConnected = (ITF::PluginGateWay::getSingleton()->init(myAddress, port, ITF::bfalse,ITF::bfalse) == ITF::btrue);

    SF_DEL_ARRAY(myAddress);
    return m_bIsConnected;
}

//------------------------------------------------------------------------------------------------------------------------------------------------

void engineWrapper::getFileOpen(Dictionary< String^,int> ^%_openedFiles)
{
    ITF::FileManager_ITF *pFileManagerITF =(ITF::FileManager_ITF*) ITF::Singletons::get().getFileManager();

    if (pFileManagerITF)
    {
        const ITF_MAP<ITF::String_Uncached,ITF::u32>& mapFiles = pFileManagerITF->getFilesOpenedGlobal();
     
        ITF_MAP<ITF::String_Uncached,ITF::u32> ::const_iterator iter;
        for (iter= mapFiles.begin();iter!= mapFiles.end();iter++)
        {
            String^ key =  gcnew String((wchar_t*)(*iter).first.cStr());
          
            _openedFiles[key] = (*iter).second;
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------


void engineWrapper::copyToClipboard(blobWrapper^ _blob)
{
    int uLabelFormat = RegisterClipboardFormat(L"ITF_CLIPBOARD"); 
    int err=   0;
    CloseClipboard(); 
    if ( !OpenClipboard(NULL) )
    {
    }

    err=   GetLastError();

    // Remove the current Clipboard contents
    if( !EmptyClipboard() )
    {  
        err=   GetLastError();
    }
    // ...
    // Get the currently selected data
    // ...
    // For the appropriate data formats...

    int blobSize = _blob->getBlob()->getSize();
    HGLOBAL  hglbCopy = GlobalAlloc(GMEM_MOVEABLE,  blobSize+sizeof(int)); 
    byte*  lptstrCopy; 

    lptstrCopy =(byte*) GlobalLock(hglbCopy); 
    memcpy(lptstrCopy,&blobSize , sizeof(blobSize)); 
 
    memcpy(lptstrCopy+sizeof(int),_blob->getBlob()->getData() , blobSize); 

    GlobalUnlock(hglbCopy); 


    if (hglbCopy == NULL) 
    { 
        CloseClipboard(); 
    } 

    if ( ::SetClipboardData(uLabelFormat,hglbCopy ) == NULL )
    {
        ::SetClipboardData(uLabelFormat,hglbCopy );
        CloseClipboard();
        err=   GetLastError();

    }
    // ...
    CloseClipboard();

}

//------------------------------------------------------------------------------------------------------------------------------------------------

//to use for some specific plugin
void engineWrapper::initFilemanager()
{
    ITF::FileManager_ITF *pFileManagerITF = new  ITF::FileManager_ITF();
    ITF::Singletons::get().setFileManager(pFileManagerITF);
    pFileManagerITF->m_bBundleSupported = ITF::bfalse;
    
}

//------------------------------------------------------------------------------------------------------------------------------------------------

void engineWrapper::addPlugin(String^ _name,pluginWrapper^ _plugin)
{
   PluginReceiver* ptr_plugin = new PluginReceiver();
   ITF::PLUGINGATEWAY->addPlugin(utils::getITFString(_name),ptr_plugin);
   _plugin->attach(ptr_plugin);
}

//------------------------------------------------------------------------------------------------------------------------------------------------

void engineWrapper::update()
{
    ITF::PluginGateWay* pluggateway = ITF::PLUGINGATEWAY;
    if(pluggateway)
        pluggateway->update();
}

//------------------------------------------------------------------------------------------------------------------------------------------------

uint engineWrapper::computeEngineSignature()
{
    ITF::EngineSignature::computeEngineSignature();
    return ITF::EngineSignature::getEngineSignature();
}

//------------------------------------------------------------------------------------------------------------------------------------------------

// Use enumToInt instead of a direct cast to avoid a stupid warning with the value ePolyLine (0)
template <typename T>
static uint32_t enumToInt(T val)
{
	return static_cast<uint32_t>(val);
}

#define ITF_HASH_CLASSNAME(__enum)  _dstHashClass[enumToInt(ITF::BaseObject::__enum)] = gcnew String(#__enum);


void engineWrapper::fillClassName(Hashtable ^%_dstHashClass)
{
    ITF_HASH_CLASSNAME(ePolyLine);
    ITF_HASH_CLASSNAME(eRessource);
    ITF_HASH_CLASSNAME(eFrise);
    ITF_HASH_CLASSNAME(eActor);
    ITF_HASH_CLASSNAME(eTrajectory);
    ITF_HASH_CLASSNAME(eScene);
    ITF_HASH_CLASSNAME(eSelection);
    ITF_HASH_CLASSNAME(eEditorDummy);
    ITF_HASH_CLASSNAME(eInvalid);
}

//------------------------------------------------------------------------------------------------------------------------------------------------

void engineWrapper::extractProfileData(const byte * _byte,uint size,List<profileInfoWrapper^> ^%_dstInfo,List<profileStateWrapper^> ^%_dstState)
{
    using namespace ITF;
    ArchiveMemory arch(_byte,size);
    profileScope ps;

    u32 count = 0;
    arch.serialize(count);

    String8         friendlyName;
    ITF::String     filename;

    ITF_MAP<u32,profileInfo> mMap;

    f32 timeMS;
    u32 nullObject = 0;
    for (u32 index = 0;index<count;index++)
    {
        filename.clear();
        ps.serializeRead(arch,friendlyName,filename,timeMS);

        if (ps.m_ref == ITF::ObjectRef::InvalidRef.getValue())
        {
            nullObject++;
        }

        ITF_MAP<u32,profileInfo> ::iterator iter = mMap.find(ps.m_ref);
        
        if (iter == mMap.end())
        {
            profileInfo pi;
            pi.objectRef = ps.m_ref;
            if (ps.m_typeScope == ITF::profileScope_Update)
                pi.m_deltaUpdateMS = timeMS;
            else if (ps.m_typeScope == ITF::profileScope_Draw)
                pi.m_deltaDrawMS = timeMS;
            else if (ps.m_typeScope == ITF::profileScope_DrawGPU)
                pi.m_deltaDrawGPUMS = timeMS;

            if (friendlyName.getLen()>0)
            {
                pi.m_friendlyName = friendlyName;
            }

            if (filename.getLen()>0)
            {
                pi.m_filename = filename;
            }

            pi.m_typePickable = ps.m_typePickable;
            pi.m_nbComponents = ps.m_nbComponents;
            pi.m_nbBones = ps.m_nbBones;
            pi.m_updateType = ps.m_updateType;
            pi.m_skipUpdate = ps.m_skipUpdate;
            pi.m_skipDraw = ps.m_skipDraw;
            mMap[ps.m_ref] = pi;
        }
        else
        {
            profileInfo& pi =(*iter).second;
            if (ps.m_typeScope == ITF::profileScope_Draw)
                pi.m_deltaDrawMS += timeMS;
            else if (ps.m_typeScope == ITF::profileScope_DrawGPU)
                pi.m_deltaDrawGPUMS += timeMS;

            pi.m_hit++;
        }
    }

     for (ITF_MAP<u32,profileInfo> ::const_iterator iter = mMap.begin();iter!=mMap.end();iter++)
     {
         const profileInfo& pi =  (*iter).second;

        profileInfoWrapper^ pif = gcnew  profileInfoWrapper();
        pif->objectRef          = pi.objectRef;
        pif->m_friendlyName     = gcnew System::String((wchar_t*)pi.m_friendlyName.cStr());
        pif->m_deltaDrawMS      = pi.m_deltaDrawMS;
        pif->m_deltaUpdateMS    = pi.m_deltaUpdateMS;
        pif->m_deltaDrawGPUMS   = pi.m_deltaDrawGPUMS;
        pif->m_typePickable     = pi.m_typePickable;
        pif->m_nbComponents     = pi.m_nbComponents;
        pif->m_nbBones          = pi.m_nbBones;
        pif->m_nbHit            = pi.m_hit;
        pif->m_updateType       = pi.m_updateType;
        pif->m_skipDraw         = pi.m_skipDraw;
        pif->m_skipUpdate       = pi.m_skipUpdate;

        pif->m_filename = gcnew System::String((wchar_t*)pi.m_filename.cStr());
        _dstInfo->Add(pif);
     }

     u32 countState = 0;
     arch.serialize(countState);

     for (u32 index =0;index<countState;index++)
     {
         profileStateWrapper^ pState = gcnew  profileStateWrapper();
         u32 objectRef, enableUpdate,enableDraw = 0;
         arch.serialize(objectRef);
         arch.serialize(enableUpdate);
         arch.serialize(enableDraw);

         _dstState->Add(pState);
     }
}



//------------------------------------------------------------------------------------------------------------------------------------------------

#if defined(ITF_WIN32) && !defined(ITF_FINAL)
// Fake methods to enable linking
void ITF::ObjectGroup::update( ITF::f32 /*_elapsed*/ )
{
}

void ITF::ObjectGroup::draw(SafeArray<class LightComponent*>& /*_activeLights*/ )
{
}
#endif
