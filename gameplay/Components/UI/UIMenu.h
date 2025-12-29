#ifndef _ITF_UIMENU_H_
#define _ITF_UIMENU_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    #define input_actionID_Up ITF_GET_STRINGID_CRC(MENU_UP,501263146)
    #define input_actionID_Down ITF_GET_STRINGID_CRC(MENU_DOWN,1280587374)
    #define input_actionID_Right ITF_GET_STRINGID_CRC(MENU_RIGHT,532126592)
    #define input_actionID_Left ITF_GET_STRINGID_CRC(MENU_LEFT,2917793312)

    #define input_actionID_UpHold ITF_GET_STRINGID_CRC(MENU_UP_HOLD,4172004239)
    #define input_actionID_DownHold ITF_GET_STRINGID_CRC(MENU_DOWN_HOLD,3693162641)
    #define input_actionID_RightHold ITF_GET_STRINGID_CRC(MENU_RIGHT_HOLD,3422970606)
    #define input_actionID_LeftHold ITF_GET_STRINGID_CRC(MENU_LEFT_HOLD,747984973)

    #define input_actionID_Valid ITF_GET_STRINGID_CRC(MENU_VALID,1084313942)
    #define input_actionID_Start ITF_GET_STRINGID_CRC(MENU_START,56410331)
    #define input_actionID_Back ITF_GET_STRINGID_CRC(MENU_BACK,2477582220)
    #define input_actionID_Other ITF_GET_STRINGID_CRC(MENU_OTHER,2656090922)
    #define input_actionID_DeleteSave ITF_GET_STRINGID_CRC(MENU_DELETE_SAVE,349253420)
    #define input_actionID_DeleteSaveNoEffect ITF_GET_STRINGID_CRC(MENU_DELETE_SAVE_NO_EFFECT,1963018150)
    #define input_actionID_onPressed ITF_GET_STRINGID_CRC(MENU_ONPRESSED,185785632)
    #define input_actionID_onReleased ITF_GET_STRINGID_CRC(MENU_ONRELEASED,14213630)
    #define input_actionID_UploadSave   ITF_GET_STRINGID_CRC(MENU_UPLOAD_SAVE, 3018307802)
    #define input_actionID_DownloadSave ITF_GET_STRINGID_CRC(MENU_DOWNLOAD_SAVE, 3372378238)
    ///////////////////////////////////////////////////////////////////////////////////////////

    class UIComponent;
    class UITextBox;

    class UIMenu
    {
    public:
        UIMenu  (const StringID &_menuID);
        ~UIMenu (       );

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Add an UIComponent to the menu
        /// @param _ref UIComponent reference
        void                addUIComponent          (ObjectRef _ref);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Remove an UIComponent of the menu
        /// @param UIComponent*  _uiComponent
        void                removeUIComponent       (UIComponent*  _uiComponent);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Get the current UIComponent selected in the menu
        UIComponent*        getUIComponentSelected  (       ) const;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Restore the default selection (select the default selected UIComponent and unselect the others)
        void                reinitMenuSelection     (       );
        void                setAlwaysMenuSelected   ( bbool _status) { m_AlwaysStaySelected = _status; }

        StringID            getMenuID               (       ) const { return m_menuID; }
        ObjectRefList       getUIComponentsList     (       ) const { return m_UIComponentsList; }
        void                setMenuFriendly         (String8 _friendly) { m_menuFriendly = _friendly; }
        ObjectRef           getWorldMenuRef         (       ) const { return m_worldMenuRef; }
        void                setWorldMenuRef         (ObjectRef _worldMenuRef) { m_worldMenuRef = _worldMenuRef; }

        UIComponent*        getUIComponentByFriendly(const String8& _friendly);
        UIComponent*        getUIComponentByID(const StringID& _id);
        UITextBox*          getUITextBoxByFriendly(const String8& _friendly);
        Actor*              findActorInMenuWorldByUserFriendly(const char* userFriendly);
        ///////////////////////////////////////////////////////////////////////////////////////////
        ///
        void                enable() { m_active = btrue; }
        void                disable() { m_active = bfalse; }
        bbool               isEnable() const { return m_active; }

        bbool               isPadAllowed(u32 _padIndex);
        void                setPadTypeAllowed(u32 _padType);


        ITF_INLINE void     setAllowedIndexPadOnly(u32 _padIndex){ m_forcePadIndex = _padIndex; }
        ITF_INLINE u32      getAllowedIndexPadOnly() const { return m_forcePadIndex; }

    private:
        String8             m_menuFriendly;
        StringID            m_menuID;
        ObjectRefList       m_UIComponentsList;
        ObjectRef           m_worldMenuRef;
        bbool               m_AlwaysStaySelected;
        u32                 m_forcePadIndex;
        bbool               m_active;
        u32                 m_padTypeAllowed;
    };
}
#endif // _ITF_UIMENUITEMTEXT_H_
