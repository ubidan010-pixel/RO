#ifndef _ITF_PLUGIN_TEXTEDITOR_H_
#define _ITF_PLUGIN_TEXTEDITOR_H_

#ifndef _ITF_PLUGIN_H_
#include "PluginGateway/Plugin.h"
#endif //_ITF_PLUGIN_H_

namespace ITF
{

class TODOComponent;

///////////////////////////////////////////////////////////////////////////////////////////
class Plugin_TextEditor : public Plugin
{
public:
    static const char*      getPluginName               (   ) {return  "TextEditor_Plugin";}
    static const char*      getTextEnteredCmdName       (   ) {return  "TextEntered";}
    static const char*      getAliveCmdName             (   ) {return  "Alive";}
    static const char*      getCloseCmdName             (   ) {return  "Close";}
    static const char*      getHideCmdName              (   ) {return  "Hide";}
    static const float      getUpdateRate               (   ) {return 0.25f;} // in seconds

                            Plugin_TextEditor       (   );
    void                    update                  (   float _ellapsed );

    // Plugin inherited
    void                    receive        (   Blob& _blob, const NETPeer* _peer, u32 _senderID );
    void                    update         (               );
    void                    onActorPicked  (    ObjectRef /*_actor*/);
    TODOComponent*          getCurrentTODO ();
    void                    onToolConnect(const NETPeer* _peer, u32 _senderID);
    void                    onCloseApp( );
    void                    onMouseButton  (   InputAdapter::MouseButton _but, InputAdapter::PressStatus _status );

private:
    void                    showMyActor();
    void                    onTextEntered   (   Blob& _blob  );
    ObjectRef               m_curActor, m_lastShownActor;
    f64                     m_lastAlive;
};

} // namespace ITF


#endif // _ITF_PLUGIN_TEXTEDITOR_H_
