#include "core/enginecommon.h"

#include "filePacker.h"

#ifndef _ITF_PUGIXMLWRAP_H_
#include "core/XML/PugiXMLWrap.h"
#endif //_ITF_PUGIXMLWRAP_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_FILEPACKFORMAT_H_
#include "engine/file/FilePackFormat.h"
#endif //_ITF_FILEPACKFORMAT_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_DIRECTORY_H_
#include "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_

#ifndef _ITF_BUNDLEMANAGER_H_
#include "engine/file/BundleManager.h"
#endif //_ITF_BUNDLEMANAGER_H_

#ifndef _ITF_BUNDLEBUILDER_H_
#include "engine/file/BundleBuilder.h"
#endif //_ITF_BUNDLEBUILDER_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_BUNDLEFILE_H_
#include "engine/file/BundleFile.h"
#endif //_ITF_BUNDLEFILE_H_

#ifndef _ITF_FILEPACKFORMAT_H_
#include "engine/file/FilePackFormat.h"
#endif //_ITF_FILEPACKFORMAT_H_

#ifndef _ITF_ENGINESIGNATURE_H_
#include "engine/signature/engineSignature.h"
#endif //_ITF_ENGINESIGNATURE_H_

#include <direct.h> // for getcwd
#include <stdlib.h>// for MAX_PATH
#include <iostream> // for cout and cin

namespace ITF
{

void extractFilter(const String& src,ITF_VECTOR<String>& _vFilter)
{
  const u16* buffer = src.cStr();
  const u16* current = buffer;

  while (*buffer)
  {
      current = buffer;
      while (*buffer != ';' && *buffer)
         buffer++;
     
      _vFilter.push_back(String(current, u32(buffer-current)));

      if (!*buffer)
          break;

      buffer++;
  }
}


bbool isAllowedFileOnPlatform(const String& _szFilename,const String& _szPlatform)
{
    String cookedShaderExt = "shaders/compiled/";
    cookedShaderExt += (u16*)_szPlatform.cStr();
    cookedShaderExt.toLower();
    bool isCompiledShader = _szFilename.strstr((u16*)(cookedShaderExt.cStr())) != NULL;
    const String& cookDepot = FILESERVER->getCookDepot();
    if (_szFilename.strstr((u16*)(cookDepot.cStr()))==NULL
        && !isCompiledShader )
    {
        return bfalse;
    }

    return btrue;
}

static BundlePlatform::Enum getBundlePlatformFromString(const String & _strPlatform)
{
    BundlePlatform::Enum uPlatform = BundlePlatform::PC;

    if (_strPlatform == "X360")
    {
        uPlatform = BundlePlatform::X360;
    }
    else if (_strPlatform == "PS3")
    {
        uPlatform = BundlePlatform::PS3;
    }
    else if (_strPlatform == "IPAD")
    {
        uPlatform = BundlePlatform::IPAD;
    }
    else if (_strPlatform == "CTR")
    {
        uPlatform = BundlePlatform::_CTR;
    }
    else if (_strPlatform == "WII")
    {
        uPlatform = BundlePlatform::WII;
    }
    else if (_strPlatform == "PS5")
    {
        uPlatform = BundlePlatform::PS5;
    }
    else if (_strPlatform == "NX")
    {
        uPlatform = BundlePlatform::NSwitch;
    }
    else if (_strPlatform == "OUNCE")
    {
        uPlatform = BundlePlatform::NSwitch2;
    }
    return uPlatform;
}

//this function assume the files added are allowed
bbool FilePacker::makeForDLL(const String& _szRoot,const String& _absoluteDstFilename,const String& _szPlatform,int _count,const String** _vFiles,callbackProgressBuild _func,int _dataversion)
{
    LOG("FilePacker started for :%ls",_szPlatform.cStr());
    
    BundleBuilder bundleBuild(_szRoot);

    int  count =_count;
    FILESERVER->rebuildCookDepot(_szPlatform);
    //DATA DIRECTORY

    for (int index=0 ;index<count;index++)
    {
        const String *ptr =_vFiles[index];
        String file = *ptr;
        file = FilePath::normalizePath(file);
        file.toLower();
        bundleBuild.addFile(file);
    }

    u32 uPlatform = getBundlePlatformFromString(_szPlatform);

    //String filename;
    //filename.setTextFormat("%ls%ls.ipk",_absoluteDstFilename, _szPlatform.cStr());
    bundleBuild.generate(_absoluteDstFilename,uPlatform,bfalse,_func,_dataversion);
    LOG("\nFilePacker finished");

    return true;
}

bbool FilePacker::makeGlobalFatForDll( const String& root, const String& platform, const String& filename, const String& bundlesOutput, const String** bundleNames, u32 count )
{
    LOG( "FilePacker global fat build started" );

    GlobalFatBuilder    builder;
    GlobalFat           fat;
    BundleFile          bundle;
    ITF_VECTOR< Path >  files;
    u32                 signature = EngineSignature::getEngineSignature();
    String              normOutput = FilePath::normalizePath( bundlesOutput );
    String              bundleFullPath;

    if ( normOutput.cStr()[normOutput.getLen() - 1] != '/' )
        normOutput += '/';
    for ( u32 i = 0; i < count; ++i )
    {
        SharableBundleHeader headers( root );

        bundleFullPath.setTextFormat( "%ls%ls_%ls.ipk", normOutput.cStr(), bundleNames[i]->cStr(), platform.cStr() );
        bundle.openBundle( bundleFullPath, &headers, 0, btrue, signature, bfalse );
        headers.getFiles( files );
        for ( u32 j = 0; j < files.size(); ++j )
            builder.referenceFile( files[j], *bundleNames[i] );
        bundle.closeBundle();
    }
    return builder.save( filename );
}

bbool FilePacker::make(const String& _szRoot,const String& _szPlatform,const ITF_VECTOR<String>& _vFiles)
{
    LOG("FilePacker started for :%S",_szPlatform.cStr());

    wchar_t  workingDirectory[_MAX_PATH];
    _wgetcwd(workingDirectory,_MAX_PATH);

    String szFilenameConfig;
    szFilenameConfig.setTextFormat("coreConfig/configPlugin/platformSettingsInstaller%ls.xml",_szPlatform.cStr());

    //FILESERVER->setWorkingDepot((u16*)workingDirectory);

    PugiXMLWrap XML;
    if (!XML.openFile(szFilenameConfig, bfalse, PugiXMLWrap::DoNothing))
    {
        LOG("FilePacker missing configuration file");
        return bfalse;
    }

    String srcFilters      = "";
    String srcFiltersCook  = "";

    ITF_SET<String> mFiltersAllowed;

    if (XML.enterElement("UserSettings"))
    {
        do
        {
            srcFilters     = XML.readAttribute("Filters","");
            srcFiltersCook = XML.readAttribute("FiltersCook","");

        } while (XML.nextSameElement());
        XML.exitElement();


        ITF_VECTOR<String> vFilters;
        extractFilter(srcFilters,vFilters);
        extractFilter(srcFiltersCook,vFilters);

        for (ITF_VECTOR<String>::const_iterator iterFilter = vFilters.begin();iterFilter!=vFilters.end();++iterFilter)
        {
            mFiltersAllowed.insert(*iterFilter);
        }

    }

    ITF_VECTOR<String> vStrFilenameExcluded;
    vStrFilenameExcluded.push_back("lu_.xml");


    BundleBuilder bundleBuild(_szRoot);

    FILESERVER->rebuildCookDepot(_szPlatform);
    //DATA DIRECTORY
    for (ITF_VECTOR<String>::const_iterator iter= _vFiles.begin();iter!=_vFiles.end();++iter)
    {
        String file = (*iter);
        file = FilePath::normalizePath(file);
        file.toLower();
        String ext = FilePath::getExtension(file);

        bbool allowed = btrue;

        if ((ext == "ckd" || (ext == "bin" && _szPlatform != "CTR")) && !isAllowedFileOnPlatform(file,_szPlatform))
        {
            allowed = bfalse;
        }

        if (allowed && mFiltersAllowed.find(ext) != mFiltersAllowed.end())
        {
            for (ITF_VECTOR<String>::const_iterator iterStr = vStrFilenameExcluded.begin();iterStr!=vStrFilenameExcluded.end();++iterStr)
            {
                if (file.strstr((u16*)(*iterStr).cStr())!=NULL)
                {
                    allowed = bfalse;
                    break;
                }
            }
        }
        else
        {
            allowed = bfalse;
        }


        if (allowed)
        {
            bundleBuild.addFile(file);
        }

    }

    //now we have a clean list

    //work with it;
    String filename;
    u32    uPlatform = getBundlePlatformFromString(_szPlatform);
    filename.setTextFormat("bundle_%S.ipk", _szPlatform.cStr());
    bundleBuild.generate(filename,uPlatform,bfalse,NULL,0);
    LOG("\nFilePacker finished");

    return btrue;
}


}
