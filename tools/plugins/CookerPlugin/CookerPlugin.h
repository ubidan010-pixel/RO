#ifndef _ITF_COOKERPLUGIN_H_
#define _ITF_COOKERPLUGIN_H_

#ifndef _ITF_PLUGIN_H_
#include "PluginGateway/Plugin.h"
#endif //_ITF_PLUGIN_H_

namespace ITF
{

class cookerFactory;

class CookerPlugin : public Plugin
{
public:
    CookerPlugin();
    virtual ~CookerPlugin();

    static const char*      getPluginName() {return  "Cooker_Plugin";}
    static const float      getUpdateRate() {return 1.0f;} // in seconds

    static void addVersionExtension(const String& _szFilename,const String& _szPlatform,String& _szVersionExtension);

    static void requestCooking(const String& _filename);
    static bbool requestNeededCooking(const String& _filename,bbool _HResVersion);
    static void cookHRes(const String& _filename);

    static bbool cudaIsEnable();

    void requestCookingInternal(const String& _filename);
    bbool requestNeededCookingInternal(const String& _filename,bbool _HResVersion);

    // Plugin inherited
    virtual void            receive( class Blob& _blob, const class NETPeer* _peer, u32 _senderID );
    virtual void            update();

    static CookerPlugin*    get()       {return m_pInstance;}

    const bbool             isCooking() const;
    void                    getCookFile(String& _result);
    void                    onKey( i32 _key, InputAdapter::PressStatus _status );
    void                    toggleShowDebug() { m_showDebug = !m_showDebug; }
 
    void cookHResInternal(const String& _filename);

    cookerFactory* getFactory() const { return m_cookFactory; }

private:

    ITF_THREAD_CRITICAL_SECTION     m_cs;
    cookerFactory*                  m_cookFactory;
    static CookerPlugin*            m_pInstance;
    bbool                           m_showDebug;
	String					        m_platform;
   
};

} // namespace ITF


#endif // _ITF_COOKERPLUGIN_H_
