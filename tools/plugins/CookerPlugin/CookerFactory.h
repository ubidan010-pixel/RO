#ifndef _ITF_COOKERFACTORY_H_
#define _ITF_COOKERFACTORY_H_

#ifndef _ITF_RESOURCE_H_   
#include "engine/resources/Resource.h"
#endif //_ITF_RESOURCE_H_  

namespace ITF
{
    class DepCollection;
    class DepCollector;

    typedef bbool (*funcDependencies)( const String& cacheKey, const String& _vPlatform,const String& _szFilename,DepCollection& _vDependencies );
    typedef bbool (*func_cook)(const String& _vPlatform,const String& _szFilename);

    struct cookInformation
    {
        cookInformation():m_funcCook(NULL),m_funcDependencies(NULL),m_version(U32_INVALID),m_delayedCook(bfalse) {}

        func_cook           m_funcCook;
        funcDependencies    m_funcDependencies;
        u32                 m_version;
        bbool               m_delayedCook;
    };

    class cookerFilter
    {
    public:
        cookerFilter(const String& _extension,func_cook _funkCook,funcDependencies _funcDependencies,u32 _version,bbool _delayedCook):m_Extension(_extension)
        {
            m_cookInformation.m_version   = _version;
            m_cookInformation.m_funcCook  = _funkCook;
            m_cookInformation.m_delayedCook = _delayedCook;
            m_cookInformation.m_funcDependencies = _funcDependencies;
        };

        bbool call(const String& _platform,const String& _filename)
        {
            if ( m_cookInformation.m_funcCook != NULL )
                return m_cookInformation.m_funcCook(_platform,_filename);
            return bfalse;
        }

        bbool callDependencies( const String& cacheKey, const String& _platform,const String& _filename,DepCollection& _vDependencies )
        {
            if (m_cookInformation.m_funcDependencies)
            {
                return m_cookInformation.m_funcDependencies(cacheKey, _platform,_filename,_vDependencies);
            }

            return bfalse;
        }

        const cookInformation& getCookInformation() const {return m_cookInformation;}

    protected:

        cookInformation m_cookInformation;
        String          m_Extension;
    };



    class cookerFactory
    {
    public:

        cookerFactory();
        virtual ~cookerFactory();
       
        void runInBackground();
        void registerCooker(const String& _extension, func_cook  _funcCook,funcDependencies _funcDependencies,u32 _version,bbool _delayedCook = bfalse);
        void registerCookerHRes(const String& _extension, func_cook  _funcCook,u32 _version);
        void cook(const String& _platform,const String& _filename);
        void cookHRes(const String& _filename);
        bbool directCook(const String& _platform,const String& _filename);

        bbool isCookable(const String& _filename);
        
		String getPattern() const;
        
		void getExtensionVersion(const String& _filename,const String& _platform,String& _dst); 
        void update();
        f64  getGlobalCookingTime() { return m_globalCookingTime; }
        void init(bbool modeAsync = btrue);

        void getDependencyFiles(const String& _filename, const String& _platform, DepCollection& _dependencies);
        void getDependencyFiles(const String& _filename, const String& _platform, DepCollector& _dependencies);
        void resolveDependencyFiles(const String& _platform, DepCollector& _dependencies);

        static void	    deleteOldVersion(const String& _filename,const String& _platform);
        static bbool    fileNeedsCook   (const String & _file, const String & _cookFile,const String& _platform,u32 _version =  U32_INVALID);

        static u32      getVersionFile(const String& _szFilename);

        void   getCurrentlyCookedFileName(String& _result);

		const bbool     isCooking() const           {return m_isCooking;}
        void            beginCooking(const String& _szFilename);
        void            endCooking();

        static void     setForceMode(bbool modeForce)                           {m_bForceMode = modeForce;}        
        static bbool    getForceMode()                                          {return m_bForceMode;}

        //tells if this platform version can be skipped
        static bbool    skipVersioningPlatform(const String& _szPlatform);

        static bbool                            m_bForceVersioningCheck;

        static cookerFactory* getInstance() { return m_pInstance; }

        void            setCacheKey( const String& cacheKey ) { m_cacheKey = cacheKey; }
        const String&   getCacheKey() const { return m_cacheKey; }

    protected:
        String                                  m_currentFileBeingCooked;
        f64                                     m_globalCookingTime;

        ITF_THREAD_CRITICAL_SECTION             m_cs;
		ITF_THREAD_CRITICAL_SECTION				m_csCooking;
        ITF_THREAD_CRITICAL_SECTION				m_csCookingNaming;
        ITF_THREAD_EVENT                        m_cookEvent;
        ITF_THREAD_EVENT                        m_cookEventExit;

        String                                  m_currentCook;    
		
        volatile bbool                          m_allowExit;

        class ResourceGroup*                    m_cookingGroup;

        ITF_MAP<String,cookerFilter*>           m_Factory;
        ITF_MAP<String,cookerFilter*>           m_FactoryHRes;

        ITF_VECTOR<String>                      m_listCookPending;

        bbool                                   m_bModeAsync;
        bbool                                   m_isCooking;
        static bbool                            m_bForceMode;		

	private:
		void			cookInternal(cookerFilter* ptrCookerFilter,const String& _platform,const String& _filename);

        static cookerFactory*                   m_pInstance;
        String                                  m_cacheKey;
    };

} // namespace ITF


#endif // _ITF_COOKERFACTORY_H_
