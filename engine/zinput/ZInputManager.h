#ifndef _ITF_ZINPUT_MANAGER_H
#define _ITF_ZINPUT_MANAGER_H

#ifndef _ITF_ACTIONMAP_H_
#include "engine/zinput/ZActionMap.h"
#endif //_ITF_ACTIONMAP_H_

#ifndef ITF_TEMPLATECLIENTHANDLER_H
#include "engine/TemplateManager/TemplateClientHandler.h"
#endif // ITF_TEMPLATECLIENTHANDLER_H

namespace  ITF
{
    class IInputListener;
    class IInputDevice;
    class ZInputManager_Template;
    class ZInputManager
    {

    public:
        ZInputManager();
        ~ZInputManager();

        enum Category
        {
            Cat_GamePlay=0,
            Cat_Cheat,
            Cat_Menu,
            Cat_Count
        };

        void                load_configFile( const Path& _path );
        void                update();
        void                setActionMapsActive(const Category _category, bbool _actif);

        /// Devices controller
        void                addX360pad_device(u32 maxPAD);
        void                addWiimote_device(u32 maxPAD);
        void                addWiinunchuck_device(u32 maxPAD);
        void                addWiiclassic_device(u32 maxPAD);
        void                addPS3pad_device(u32 maxPAD);
        void                addPS5pad_device(u32 maxPAD);
        void                addNintendopad_device(u32 maxPAD);
        void                addXBoxSeries_device(u32 maxPAD);

        struct ActionMapInternal
        {
            ActionMap _actionMap;
            StringID  _config;
            Category  _category;
            bbool _actif;
        };

        void                AddListener(IInputListener* _listener, const Category _category = Cat_GamePlay);
        void                RemoveListener(IInputListener* _listener) ;

        void                ForceConfigForPlayer(u32 _player, const StringID & _config);
    private:
        typedef ITF_VECTOR<IInputListener*> Listeners;
        Listeners   m_listeners;
        Listeners   m_listenersRemoved;

        typedef ITF_VECTOR<Listeners>    ListOfListeners;
        ListOfListeners     m_listOfListeners;

        typedef ITF_MAP <StringID, ActionMapInternal> ActionMapsContainer;
        ActionMapsContainer     m_actionMaps;

        typedef ITF_VECTOR<IInputDevice*> Devices;
        Devices m_devices;
        TemplateClientHandler   m_templateClientHandler;

        void                remove_fromList(IInputListener* _listener);
        void                clean_removedListeners();
        void                update_actionMap(ActionMapInternal* _actionMapInternal);
        void                process_actionDevices(ActionMapInternal* _actionMapInternal);
        void                update_devices ();

        StringID            m_playersConfig[ACTIONMAP_MAX_PLAYER];
    };


    class ZInputManager_Template : public TemplateObj
    {
        DECLARE_OBJECT_CHILD_RTTI(ZInputManager_Template,TemplateObj,3594148990);
        DECLARE_SERIALIZE()
        ZInputManager_Template()
            : m_actif(btrue), m_category(0), Super(){}

        ZInputManager_Template(const Path& _path)
            : m_actif(btrue), m_category(0), Super(_path){}


        ITF_INLINE const ActionMap & getActionMap()const { return m_actionMap; }
        ITF_INLINE const StringID & getName()const { return m_name; }
        ITF_INLINE const StringID & getConfig()const { return m_config; }
        ITF_INLINE const bbool getActif() const { return m_actif; }
        ITF_INLINE const u32 getCategory() const { return m_category; }
    private:
        ActionMap               m_actionMap;
        StringID                m_name, m_config;
        u32                     m_category;
        bbool                   m_actif;
    };

} // namespace  ITF

#endif // _ITF_ZINPUT_MANAGER_H
