#ifndef _ITF_LINKEDITOR_H_
#define _ITF_LINKEDITOR_H_

#ifndef _ITF_PLUGIN_H_
#include "PluginGateway/Plugin.h"
#endif //_ITF_PLUGIN_H_

namespace ITF
{

    class Blob;
    class ChildEntry;

    class LinkEditor : public Plugin
    {
    public:
        static const char*      getPluginName       ( )                         { return "LinkEditor_Plugin"; }
        static const char*      getClearCmdName     ( )                         { return "Clear"; }
        static const char*      getFillCmdName      ( )                         { return "Fill"; }
        static const char*      getUpdateCmdName    ( )                         { return "Update"; }

        // Plugin inherited
        virtual void            update              ( );
        virtual void            receive             ( Blob& _blob, const NETPeer* _peer, u32 _senderID );
        virtual void            onCloseApp          ( );
        virtual void            onMouseButton       ( InputAdapter::MouseButton _but, InputAdapter::PressStatus _status );
        virtual void            onObjectChanged     ( BaseObject* _pObject );
        virtual void            UnselectAll         ( bbool _force );
        virtual bbool           isPickingAllowed    ( );
        virtual void            onToolConnect       (const NETPeer* _peer, u32 _senderID);

    private:
        void                    sendFill(ObjectRef ownerRef, const ChildEntry& _childEntry);
        void                    sendClear();
        void                    receiveUpdateTags(Blob& _blob);
        ObjectRef               m_hoveredLinkParent;
        ObjectRef               m_hoveredLinkChild;

        ObjectRef               m_viewedActor;
        ObjectPath              m_viewedLink;
    };

} // namespace ITF


#endif // _ITF_LINKEDITOR_H_
