#include "precompiled_core.h"

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif //_ITF_VERSIONING_H_

#ifndef _ITF_ARCHIVE_H_
#include "core/Archive.h"
#endif //_ITF_ARCHIVE_H_

#ifndef _ITF_BLOB_H_
#include "core/Blob.h"
#endif //_ITF_BLOB_H_

namespace ITF
{
    bbool Versioning::serializeVersion(ArchiveMemory& __archive,u32 _version)
    {
        u32 serialize_version = _version;
        __archive.serialize(serialize_version);
        return (serialize_version == _version);
    }


   struct VersionResult
   {
       u32      version;
       String   versionName;
       bbool    matched;
   };

#define SERIALIZE_VERSION(tag)  version = tag();__archive.serialize( version);

   void Versioning::serializeAll(ArchiveMemory& __archive)
   {
        u32 version = 0;
        SERIALIZE_VERSION(logicDatabaseCacheVer);
        SERIALIZE_VERSION(textureVer);         
        SERIALIZE_VERSION(soundVer);              
        SERIALIZE_VERSION(animPatchBankVer);           
        SERIALIZE_VERSION(animTrackVer);                  
        SERIALIZE_VERSION(animSkeletonVer);               
        SERIALIZE_VERSION(animDependeniesVer);          
        SERIALIZE_VERSION(gameVer);           
        SERIALIZE_VERSION(bundleVer);      
        SERIALIZE_VERSION(friezeConfigVer);
        SERIALIZE_VERSION(sceneVer);             
        SERIALIZE_VERSION(atlasVer);         
   }

    void writeTagVersion(Blob& _blob,u32 _version,const String& _name,ITF_VECTOR<VersionResult>& versions)
    {
        _blob.pushUInt32(_version);
    }

    void readTagVersion(Blob& _blob,u32 _version,const String& _name,ITF_VECTOR<VersionResult>& versions)
    {
        u32 ver = _blob.extractUint32();
        VersionResult vResult;
        vResult.matched = (_version == ver);
        vResult.versionName = _name;
        vResult.version = _version;

        versions.push_back(vResult);
    }

    typedef void (*blobFunc)(Blob& _blob,u32 _version,const  String& _name,ITF_VECTOR<VersionResult>& versions);

    #define VERSION_TAG(name) {func(_blob,name(),#name,versions);}

    bbool Versioning::Versions(Blob& _blob,bbool _bWrite)
    {
        blobFunc func = NULL;
        ITF_VECTOR<VersionResult> versions;
        
        if (_bWrite) 
            func = writeTagVersion;
        else
            func = readTagVersion;

        VERSION_TAG(logicDatabaseCacheVer);
        VERSION_TAG(textureVer);
        VERSION_TAG(soundVer);
        VERSION_TAG(animPatchBankVer);
        VERSION_TAG(animTrackVer);
        VERSION_TAG(animSkeletonVer);
        VERSION_TAG(gameVer);
        VERSION_TAG(bundleVer);
        VERSION_TAG(friezeConfigVer);
        VERSION_TAG(sceneVer);

       

        return btrue;
    }


    u32 Versioning::getEngineVersion()
    {
#if  VERSION_ENGINE
        return VERSION_ENGINE;
#else // !VERSION_ENGINE
        static u32 version = 0;
    #if defined(ITF_WINDOWS)
        static bbool initialized = bfalse;
        if(!initialized)
        {
            wchar_t strExePath [MAX_PATH];
            const DWORD uSize = GetModuleFileName(NULL, strExePath, MAX_PATH);
            if(uSize)
            {
                const LPTSTR sName = (LPTSTR)strExePath;
                DWORD dwDummy;
                const DWORD dwSize = GetFileVersionInfoSize(sName, &dwDummy);

                LPBYTE lpVersionInfo =  new_array(BYTE,dwSize,mId_System);
                if(GetFileVersionInfo( sName , 0 , dwSize , lpVersionInfo ))
                {
                    UINT uLen;
                    VS_FIXEDFILEINFO *lpFfi;
                    if(VerQueryValue( lpVersionInfo , L"\\", (LPVOID *)&lpFfi , &uLen ))
                    {
                        //const DWORD dwFileVersionMS = lpFfi->dwFileVersionMS;
                        const DWORD dwFileVersionLS = lpFfi->dwFileVersionLS;

                        //const DWORD dwLeftMost     = HIWORD(dwFileVersionMS);
                        //const DWORD dwSecondLeft   = LOWORD(dwFileVersionMS);
                        //const DWORD dwSecondRight  = HIWORD(dwFileVersionLS);
                        const DWORD dwRightMost    = LOWORD(dwFileVersionLS);
                        version = u32(dwRightMost);
                    }
                    else
                    {
                        ITF_ASSERT(0);
                    }
                }
                else
                {
                   // ITF_ASSERT_MSG(0, "Can't retrieve the version from executable");  //uuntil it s properly fix
                }
                delete[] lpVersionInfo;
            }
            initialized = btrue;
        }
    #endif // ITF_WINDOWS
        return version;
#endif  //!VERSION_ENGINE
    }
}
