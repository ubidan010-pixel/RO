#include "precompiled_core.h"

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_THREAD_H_
#include "core/system/Thread.h"
#endif //_ITF_THREAD_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _INC_STDIO
    #include <stdio.h>
#endif // _INC_STDIO


namespace ITF
{

    /// Constructor.
    FileServer::FileServer():m_func_addVersionExtension(NULL),m_bModeFinal(bfalse)
#ifdef MODE_HRESTEXTURE_SUPPORTED
        ,m_HResEnable(bfalse)
#endif //MODE_HRESTEXTURE_SUPPORTED
    {
#ifndef ITF_SUPPORT_EDITOR
        m_bBinaryMode = btrue;
#else
        m_bBinaryMode = bfalse;
#endif
    }



  void FileServer::setWorkingDepot(const String& szDepotName)
  {
      m_szWorkingDepot = FilePath::normalizePath(szDepotName);

      m_szDataDepot   = m_szWorkingDepot;

      m_szDataDepot = FilePath::normalizePath(m_szDataDepot);
  }

//------------------------------------------------------------------------------------------------------------------

  void FileServer::rebuildCookDepot(const String& _szPlatform)
  {
		m_szCookedDepot = String(COOKED_DIRNAME) + _szPlatform + "/"; 
		m_szCookedDepot.toLower();
		m_currentCookPlatform = _szPlatform;
  }

  //------------------------------------------------------------------------------------------------------------------

  String FileServer::getCookedName(const String& _relativePath,bbool _useVersioning /*=btrue*/)
  {
      String szCookDirectory;

      if (getDirectoryFromCookedFolder(szCookDirectory, _relativePath))
          return szCookDirectory+getBaseName(_relativePath,_useVersioning,m_currentCookPlatform,"ckd");
      else
          return szCookDirectory;
  }

  //------------------------------------------------------------------------------------------------------------------


  String FileServer::getBaseName(const String& _relativePath,bbool _useVersioning,const String& _platform,const String& extensionAdded)
  {
      //cooked name
#ifdef ITF_WINDOWS  //because use extension
      String extension;
      if (m_func_addVersionExtension && _useVersioning)
          m_func_addVersionExtension(_relativePath,_platform,extension);

      ITF_ASSERT(m_currentCookPlatform.cStr());

      String baseName;
      baseName.setTextFormat("%ls%ls.%ls",FilePath::getFilename(_relativePath).cStr(),extension.cStr(),extensionAdded.cStr());
#else
      String baseName;
      baseName.setTextFormat("%s.%s", StringToUTF8(FilePath::getFilename(_relativePath)).get(), StringToUTF8(extensionAdded).get());
#endif //ITF_WINDOWS


      return baseName;
  }

  //------------------------------------------------------------------------------------------------------------------

  String FileServer::getCachedName(const String& _relativePath,const String& _platform, bbool _useVersioning)
  {
       String szCookDirectory;

      if (getDirectoryFromCachedFolderPlatform(szCookDirectory, _relativePath,_platform))
          return szCookDirectory+getBaseName(_relativePath,_useVersioning,_platform,"cache");
      else
          return szCookDirectory;
  }

  //------------------------------------------------------------------------------------------------------------------

  String FileServer::getCachedNamePlatform(const String& _relativePath,const String& _szPlatform,bbool _useVersioning )
  {
      String szCookDirectory;

      if (getDirectoryFromCachedFolderPlatform(szCookDirectory, _relativePath,_szPlatform))
          return szCookDirectory+getBaseName(_relativePath,_useVersioning,_szPlatform,"cache");
      else
          return szCookDirectory;

  }

  //------------------------------------------------------------------------------------------------------------------

  String FileServer::getCookedNamePlatform(const String& _relativePath,const String& _platform,bbool _useVersioning /*=btrue*/)
  {
	  String szCookDirectory;

	  if (getDirectoryFromCookedFolderPlatform(szCookDirectory, _relativePath,_platform))
		  return szCookDirectory+getBaseName(_relativePath,_useVersioning,_platform,"ckd");
	  else
		  return szCookDirectory;
  }
//------------------------------------------------------------------------------------------------------------------

    void FileServer::getAbsolutePath(const String& szFileName,String& szFullPath)const
    {
        szFullPath = m_szWorkingDepot + szFileName;
    }

//------------------------------------------------------------------------------------------------------------------

    String FileServer::getSystemRelativePath(const String& _szFilename)
    {
        return  String("ITFFRAMEWORK/")+_szFilename;
    }

    //------------------------------------------------------------------------------------------------------------------

    String FileServer::getDataRelativePath(const String& _szFilename)
    {
        return  String("DATA/")+_szFilename;
    }


    //------------------------------------------------------------------------------------------------------------------

    bbool FileServer::getPathFromDataFolder(String &_szDst,const String& _szPath, bbool _warnUser /*= btrue*/)
    {
        i32 index = -1;
        String normalizedPath = FilePath::normalizePath(_szPath);
        if (normalizedPath.strstr((u16*)m_szDataDepot.cStr(),bfalse,&index))
        {
            _szDst.setText((u16*) (normalizedPath.cStr()+index+m_szDataDepot.getLen()));
        }
        else
        {
            if(_warnUser)
            {
            #ifdef ITF_WINDOWS
                String msg("Warning! UA Framework was launched with following root folder:\n");
                msg += m_szDataDepot;
                msg += "\n\nCould not find root folder in:\n";
                msg += _szPath;
                msg += "\n\nPlease use only files within ";
                msg += m_szDataDepot;
                ITF_ERROR("File path error : %ls", msg.cStr());
            #endif // ITF_WINDOWS
            }
            _szDst = normalizedPath;
            return bfalse;
        }
        return btrue;
    }

    //------------------------------------------------------------------------------------------------------------------

    //Transform the data path folder into a cooked path folder
    bbool FileServer::getDirectoryFromCookedFolder(String &_szDst,const String& _relativePath)
    {
#ifdef ITF_IPAD
		_szDst = m_szCookedDepot;
		_szDst += _relativePath.cStr() + (_relativePath.cStr()[ 0 ] == '/');
		return btrue;
#else
        _szDst = m_szCookedDepot + FilePath::getDirectory(_relativePath);
        return btrue;
#endif     
    }

    //------------------------------------------------------------------------------------------------------------------

	bbool FileServer::getDirectoryFromCachedFolderPlatform(String &_szDst,const String& _relativePath,const String& _platform)
	{
		String CookedDepot = String(CACHE_DIRNAME) + _platform + "/"; 
		CookedDepot.toLower();
		
		_szDst = CookedDepot + FilePath::getDirectory(_relativePath);
		
		return btrue;
	}

    //------------------------------------------------------------------------------------------------------------------

    bbool FileServer::getDirectoryFromCookedFolderPlatform(String &_szDst,const String& _relativePath,const String& _platform)
    {
        String CookedDepot = String(COOKED_DIRNAME) + _platform + "/"; 
        CookedDepot.toLower();

        _szDst = CookedDepot + FilePath::getDirectory(_relativePath);

        return btrue;
    }
//------------------------------------------------------------------------------------------------------------------

    String FileServer::getTemporaryRelativePath(const String& _szFilename)
    {
        return  String("TEMP/")+_szFilename;
    }

//------------------------------------------------------------------------------------------------------------------


    void FileServer::addCookersOption(const String& _option)
    {
        if (!hasCookersOption(_option))
        {
            m_cookersOptions.push_back(_option);
        }
    }

    void FileServer::removeCookersOption(const String& _option)
    {
        auto itFound = std::find(m_cookersOptions.begin(), m_cookersOptions.end(), _option);
        if (itFound != m_cookersOptions.end())
        {
            m_cookersOptions.erase(itFound);
        }
    }

    bool FileServer::hasCookersOption(const String& _option) const
    {
        return m_cookersOptions.end() != std::find(m_cookersOptions.begin(), m_cookersOptions.end(), _option);
    }

#ifdef MODE_HRESTEXTURE_SUPPORTED
    String FileServer::getHResVersionName(const String& _relativePath)
    {
        return m_HResDirectory+ "/" +_relativePath;
    }

    //------------------------------------------------------------------------------------------------------------------

    String FileServer::getCookedHResVersionName(const String& _relativePath)
    {
        return  m_HResDirectory+ "/itf_cooked/" +_relativePath + ".ckd";
    }

#endif //MODE_HRESTEXTURE_SUPPORTED

//------------------------------------------------------------------------------------------------------------------


}
//------------------------------------------------------------------------------------------------------------------
