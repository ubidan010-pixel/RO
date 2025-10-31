#ifndef _ITF_FILESERVER_H_
#define _ITF_FILESERVER_H_

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

#include "core/system/Synchronize.h"

#define CACHE_DIRNAME "ITF_CACHE/"
#define COOKED_DIRNAME "ITF_COOKED/"

namespace ITF
{  
    class FileServer : public TemplateSingleton<FileServer>
    {

    public:


        typedef void (*func_addVersionExtension)(const String& _szFilename,const String& _szPlatform,String& _szVersionExtension);

        FileServer();

        /// Destructor
        virtual ~FileServer() {};

        /// Set a working depot where all the files will be read/write
        void setWorkingDepot(const String& szDepotName);

        void register_addVersionExtension(func_addVersionExtension _func) {m_func_addVersionExtension = _func;}

        const String& getWorkingDepot()  const          {return m_szWorkingDepot;}

        const String& getDataDepot()    const           {return m_szDataDepot;}
        
        const String& getCookDepot()    const           {return m_szCookedDepot;}

        String getSystemRelativePath(const String& _szFilename = "");

        String getDataRelativePath(const String& _szFilename = "");
        
        String getTemporaryRelativePath(const String& _szFilename = "");

        /// @fn void getAbsolutePath(const String& szRelativePath,String& szFullPath)const
        /// @brief  Gets an absolute path from the depot folder. 
        /// @param  szRelativePath    relative  pathname  relative file. 
        /// @param [out] szFullPath  full pathname . 
        void getAbsolutePath(const String& szRelativePath,String& szFullPath)const;

        bbool getPathFromDataFolder(String &_szDst,const String& _szPath, bbool _warnUser = btrue);

        bbool getDirectoryFromCookedFolder(String &_szDst,const String& _relativePath);

		bbool getDirectoryFromCookedFolderPlatform(String &_szDst,const String& _relativePath,const String& _platform);

        bbool getDirectoryFromCachedFolderPlatform(String &_szDst,const String& _relativePath,const String& _platform);

        void rebuildCookDepot(const String& _szPlatform);

        String getCookedName(const String& _szPathName,bbool _useVersioning = btrue);
        String getCachedName(const String& _szPathName,const String& _Platform,bbool _useVersioning = btrue);

		String getCookedNamePlatform(const String& _relativePath,const String& _platform,bbool _useVersioning = btrue);

        String getCachedNamePlatform(const String& _relativePath,const String& _platform,bbool _useVersioning = btrue);

		void	setModeFinal(bbool _bModeFinal) {m_bModeFinal = _bModeFinal;}

		void	setApplicationDirectory(const String& _szApplicationDirectory) {m_szApplicationDirectory = _szApplicationDirectory;}

		const String& getPlatform()  {return m_currentCookPlatform;}
		
		const String& getApplicationDirectory()	{return m_szApplicationDirectory;}

        String getBaseName(const String& _relativePath,bbool _useVersioning,const String& _platform,const String& extensionAdded);

        const bbool isBinaryMode() const {return m_bBinaryMode;}

        // Generic functions to send options to cookers
        const Vector<String> & getCookersOptions() const { return m_cookersOptions; }
        void setCookersOptions(const Vector<String>& _options) { m_cookersOptions = _options; }
        void addCookersOption(const String& _option);
        void removeCookersOption(const String& _option);
        bool hasCookersOption(const String& _option) const;

#ifdef MODE_HRESTEXTURE_SUPPORTED
        String getHResVersionName(const String& _szPathName);
        String getCookedHResVersionName(const String& _szPathName);
        const String& getHResDirectory()                                {return m_HResDirectory;}
        void setHResDirectory(const String& _HResDirectory)             {m_HResDirectory = _HResDirectory;m_HResEnable=btrue;}
        const bbool hasHResEnable()                         const       {return m_HResEnable;}
#endif //MODE_HRESTEXTURE_SUPPORTED


    protected:

        func_addVersionExtension    m_func_addVersionExtension;
        String                      m_szWorkingDepot;
        String                      m_szDataDepot;
        String                      m_szCookedDepot;
        String                      m_currentCookPlatform;
        String                      m_szApplicationDirectory;

        Vector<String>              m_cookersOptions;

#ifdef MODE_HRESTEXTURE_SUPPORTED 
        String                      m_HResDirectory;
        bbool                       m_HResEnable;
#endif //MODE_HRESTEXTURE_SUPPORTED

		bbool						m_bModeFinal;
        bbool                       m_bBinaryMode;
    };

#define FILESERVER FileServer::getptr()
}


#endif // _ITF_FILESERVER_H_
