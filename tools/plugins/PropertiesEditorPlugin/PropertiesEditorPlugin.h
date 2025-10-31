#ifndef _ITF_PROPERTIESEDITORPLUGIN_H_
#define _ITF_PROPERTIESEDITORPLUGIN_H_

#ifndef _ITF_PLUGIN_H_
#include "PluginGateway/Plugin.h"
#endif //_ITF_PLUGIN_H_

namespace ITF
{

class PropertiesEditorPlugin : public Plugin
{
public:
    PropertiesEditorPlugin();
    virtual ~PropertiesEditorPlugin();

    static const char*      getPluginName()             {return "PropertiesEditor_Plugin";}
    static const char*      getCommandPropertiesName()  {return "Properties";}
    static const char*      getCommandClearName()       {return "Clear";}
    static const char*      getCommandDumpToFileName()  {return "DumpToFile";}
    static const char*      getSessionInfoCmdName()     {return "getSessionInfo";}
    static const float      getUpdateRate()             {return 0.5f;} // in seconds

    // Plugin inherited
    virtual void            receive( class Blob& _blob, const class NETPeer* _peer, u32 _senderID );
    virtual void            onFrisePicked(ObjectRef _frise); // inherited from Plugin
    virtual void            onActorPicked(ObjectRef _actor); // inherited from Plugin

    virtual void            onObjectChanged(BaseObject* _pObject);
    void                    onBaseObjectPicked(ObjectRef _objref);

    virtual void            onToolConnect(const NETPeer* _peer, u32 _senderID);

    virtual void            UnselectAll(bbool _force);
    virtual void            update();
 
private:

    void                    sendProperties(ObjectRef _objref, const String8& _data);
    void                    sendClearSelection();
    void                    onPropertiesCmd( class Blob& _blob );
    void                    onGetSessionInfoCmd( class Blob& _blob );
    void                    onDumpToFileCmd( class Blob& _blob );

    bbool                   m_toolIsAware;
    ObjectRef               m_currentSelection;
};

} // namespace ITF


#endif // _ITF_PROPERTIESEDITORPLUGIN_H_
