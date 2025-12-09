#ifndef _ITF_ZINPUT_MANAGER_H
#define _ITF_ZINPUT_MANAGER_H

#ifndef _ITF_ACTIONMAP_H_
#include "engine/zinput/ZActionMap.h"
#endif //_ITF_ACTIONMAP_H_
#include "ZInputDevice.h"

#ifndef ITF_TEMPLATECLIENTHANDLER_H
#include "engine/TemplateManager/TemplateClientHandler.h"
#endif // ITF_TEMPLATECLIENTHANDLER_H

namespace  ITF
{
    class IInputListener;
    class IInputDevice;
    class ZInputManager_Template;
    enum EInputSourceType;
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

        enum EGameAction
        {
            Action_Up = 0,
            Action_Down,
            Action_Left,
            Action_Right,
            Action_Run,
            Action_Jump,
            Action_Hit,
            Action_Back,
            Action_Move,
            Action_Count
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
        void                addPCKeyboard_device(u32 maxPAD);

        void                SetRemap(u32 _playerIndex, u32 _logicalControl, u32 _physicalControl, EInputSourceType _source = InputSource_Gamepad);
        void                SetActionRemap(u32 _playerIndex, EGameAction _action, u32 _physicalControl, EInputSourceType _source = InputSource_Gamepad);
        bbool               IsActionRemapAllowed(EGameAction _action, u32 _physicalControl) const;
        u32                 GetStandardControlFromAction(EGameAction _action);
        u32                 GetPhysicalFromAction(u32 _playerIndex, EGameAction _action);
        i32                 GetKeyboardKeyFromAction(u32 _playerIndex, EGameAction _action);
        bbool               IsActionPressed(u32 _playerIndex, EGameAction _action) const;
        i32                 GetFirstKeyboardKey(u32 _playerIndex) const;
        void                ResetRemapping(u32 _playerIndex);
        void                ResetRemapping(u32 _playerIndex, EInputSourceType _source);
        u32                 GetFirstActiveControl(u32 _playerIndex, EInputSourceType* _outSource = nullptr);
        bbool               GetRemapping(u32 _playerIndex, EInputSourceType _source, ITF_VECTOR<u32>& _outMapping);
        void                ApplyRemapping(u32 _playerIndex, EInputSourceType _source, const ITF_VECTOR<u32>& _mapping);

        void                SetSuppressReceive(bbool _suppress) { m_suppressReceive = _suppress; }
        bbool               GetSuppressReceive() const { return m_suppressReceive; }

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
        bbool               m_suppressReceive;
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
