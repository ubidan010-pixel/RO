#ifndef _ITF_ACTORANIMPLAYERPLUGIN_H_
#define _ITF_ACTORANIMPLAYERPLUGIN_H_

#ifndef _ITF_PLUGIN_H_
#include "PluginGateway/Plugin.h"
#endif //_ITF_PLUGIN_H_

namespace ITF
{

class ActorAnimPlayerPlugin : public Plugin
{
public:
    ActorAnimPlayerPlugin();
    virtual ~ActorAnimPlayerPlugin();

    static const char*      getPluginName()             {return "ActorAnimPlayer_Plugin";}
    static const char*      getCommandAnimData()        {return "AnimData";}
    static const char*      getCommandInputValue()      {return "InputValue";}
    static const char*      getCommandActionValue()     {return "ActionValue";}
    static const char*      getCommandPauseActor()      {return "PauseActor";}
    static const char*      getCommandClear()           {return "Clear";}
    static const float      getUpdateRate()             {return 0.5f;} // in seconds

    // Plugin inherited
    virtual void            receive( class Blob& _blob, const class NETPeer* _peer, u32 _senderID );
    virtual void            onActorPicked(ObjectRef _actor); // inherited from Plugin

    virtual void            UnselectAll(bbool _force);
    virtual void            update();
 
private:

    void                    sendAnimData();
    void                    sendClearSelection();

    void                    receiveInputValue( Blob& _blob );
    void                    receiveActionValue( Blob& _blob );
    void                    receivePauseActor( Blob& _blob );

    void                    collectAnimDataFromActor( class Actor* _actor, String8& _buffer ) const;
    void                    pauseActor( bbool _pause );

    bbool                   m_toolIsAware;
    ObjectRef               m_currentSelection;
};

} // namespace ITF


#endif // _ITF_ACTORANIMPLAYERPLUGIN_H_
