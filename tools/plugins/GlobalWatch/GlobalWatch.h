#ifndef _ITF_GLOBALWATCHPLUGIN_H_
#define _ITF_GLOBALWATCHPLUGIN_H_

#ifndef _ITF_PLUGIN_H_
#include "PluginGateway/Plugin.h"
#endif //_ITF_PLUGIN_H_

namespace ITF
{

// Class predeclarations
class ColScene;
class PolyLine;
class Blob;

typedef struct GlobalWatchData
{
    GlobalWatchData     (const String& _name, const i8& _var);
    GlobalWatchData     (const String& _name, const u8& _var);
    GlobalWatchData     (const String& _name, const i16& _var);
    GlobalWatchData     (const String& _name, const u16& _var);
    GlobalWatchData     (const String& _name, const i32& _var);
    GlobalWatchData     (const String& _name, const u32& _var);
    GlobalWatchData     (const String& _name, const i64& _var);
    GlobalWatchData     (const String& _name, const u64& _var);
    GlobalWatchData     (const String& _name, const f32& _var);
    GlobalWatchData     (const String& _name, const String& _var);

    String              m_WatchName;
    Plugin::WatchType   m_WatchType;
    void*               m_WatchAdrs;
} GlobalWatchData;

class GlobalWatch : public Plugin
{
public:
    static const char*      getPluginName               (   ) {return  "GlobalWatch_Plugin";}
    static const float      getUpdateRate               (   ) {return 0.5f;} // in seconds
    static const char*      getSetValueCmdName          (   ) {return  "SetValue";}

                            GlobalWatch             (   );
    void                    update                  (   float _ellapsed );
    void                    updateWatchedValues     ( Blob& _blob  );
    float                   m_lastSentTime;
    // Plugin inherited
    void receive        (   Blob& _blob, const NETPeer* _peer, u32 _senderID );
    void update         (               );

    void                    addWatch     (const String& _name, const i32& _var);
    void                    addWatch     (const String& _name, const u32& _var);
    void                    addWatch     (const String& _name, const i16& _var);
    void                    addWatch     (const String& _name, const u16& _var);
    void                    addWatch     (const String& _name, const i8& _var);
    void                    addWatch     (const String& _name, const u8& _var);
    void                    addWatch     (const String& _name, const i64& _var);
    void                    addWatch     (const String& _name, const u64& _var);
    void                    addWatch     (const String& _name, const f32& _var);
    void                    addWatch     (const String& _name, const String& _var);
    void                    removeWatch  (const String& _name);
    bbool                   watchExists  (const String& _name);
private:
    ITF_VECTOR<GlobalWatchData> m_watch;
};

} // namespace ITF


#endif // _ITF_GLOBALWATCHPLUGIN_H_
