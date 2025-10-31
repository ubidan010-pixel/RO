#ifndef _ITF_GAMEINTERFACE_H_
#define _ITF_GAMEINTERFACE_H_

#ifndef _ITF_OBJECTFACTORY_H_
#include "core/serializer/ObjectFactory.h"
#endif //_ITF_OBJECTFACTORY_H_

#ifndef _ITF_ADAPTER_SAVEGAME_H__
#include "engine/AdaptersInterfaces/Adapter_Savegame.h"
#endif

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

#ifndef _ITF_GAMEMATERIAL_H_
#include "engine/gameplay/GameMaterial.h"
#endif //_ITF_GAMEMATERIAL_H_

#ifndef ITF_TEMPLATECLIENTHANDLER_H
#include "engine/TemplateManager/TemplateClientHandler.h"
#endif // ITF_TEMPLATECLIENTHANDLER_H

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Central interface for game. Does not depend on actual game rules.
    class GameInterface : public UIMenuManager::MenuItemActionListener
    {
    public:
        typedef bbool (*CanInterruptCutSceneCallback_Fn)(Actor *_act);
        typedef void (*CutSceneAlreadySeenCallback_Fn)(Actor *_act);

        GameInterface();
        virtual ~GameInterface();

        // Compulsary methods

        virtual void registerObjects() const = 0;
        virtual void init() = 0;

        // Optional methods

        virtual void preWorldUpdate( f32 _dt ) {}
        virtual void postWorldUpdate( f32 _dt);
        virtual void onActivatePlayer(u32 /*_playerIndex*/, class Actor* /*_player*/) {}
        virtual void onPostSceneActivation(const Scene* /*_pScene*/, bbool /*_bActive*/) {}
        virtual void updateViewsBehavior(f32 dt);
		virtual const GameMaterial_Template* allocateGameMaterial( const Path& _fileName, bool prefetch = false );
        virtual void reloadGameMaterial( const Path& _fileName ) const;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Get callback for testing if we can interrupt a cut scene
        CanInterruptCutSceneCallback_Fn getCanInterruptCutSceneCallback() const {return m_canInterruptCutSceneCallback;}
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Set the callback for testing if we can interrupt a cut scene
        void setCanInterruptCutSceneCallback(CanInterruptCutSceneCallback_Fn _callback) {m_canInterruptCutSceneCallback = _callback;}
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get the callback that must be called when a cut scene was seen completely
        CutSceneAlreadySeenCallback_Fn getCutSceneAlreadySeenCallback() const {return m_cutSceneAlreadySeenCallback;}
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// set the callback that must be called when a cut scene was seen completely
        /// @param _callback The callback. May be NULL.
        void setCutSceneAlreadySeenCallback(CutSceneAlreadySeenCallback_Fn _callback) {m_cutSceneAlreadySeenCallback = _callback;}
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get/set unskippable time for cut scenes
        void setCutSceneDefaultUnskippableDurationFirstTime(f32 _duration) {m_cutSceneDefaultUnskippableDurationFirstTime = _duration;}
        f32 getCutSceneDefaultUnskippableDurationFirstTime() const {return m_cutSceneDefaultUnskippableDurationFirstTime;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// are we showing the visual notification which shows that we are saving
        bbool   isShowingSavingNotification() const {return m_isShowingSavingNotification;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get the factory to make the game screens
        ObjectFactory *getGameScreenFactory() {return m_gameScreenFactory;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get the factory to make persistent game data
        ObjectFactory *getPersistentGameDataFactory() {return m_persistentGameDataFactory;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        // List of menu priority, by menu type, ascending order

        static UIMenuManager::MenuPriority getTutorialMenuPriority() {return UIMenuManager::MenuPriority_Lowest;}
        static UIMenuManager::MenuPriority getSequenceTextMenuPriority() {return UIMenuManager::MenuPriority_Lowest;}
        static UIMenuManager::MenuPriority getTitleScreenMenuPriority() {return UIMenuManager::MenuPriority_Lowest;}
        static UIMenuManager::MenuPriority getContextIconsMenuPriority() {return UIMenuManager::MenuPriority_Low;}
        static UIMenuManager::MenuPriority getGameMenuPriority() {return UIMenuManager::MenuPriority_Normal;}
        static UIMenuManager::MenuPriority getPauseMenuPriority() {return UIMenuManager::MenuPriority_High;}
        static UIMenuManager::MenuPriority getSavegameErrorMenuPriority() {return UIMenuManager::MenuPriority_Highest;}
        static UIMenuManager::MenuPriority getTRCMenuPriority() {return UIMenuManager::MenuPriority_Highest;}
        static UIMenuManager::MenuPriority getCriticalExitMenuPriority() {return UIMenuManager::MenuPriority_Critical;}
        static UIMenuManager::MenuPriority getNotificationMenuPriority() {return UIMenuManager::MenuPriority_Notification;}
        

        ///////////////////////////////////////////////////////////////////////////////////////////
        // Utility

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Unloading the game resources. This is for cleaning up things in order.
        void unloadGameResources();

    protected:
        ObjectFactory *m_gameScreenFactory;
        ObjectFactory *m_persistentGameDataFactory;

        //SAVING icon management
        bbool                       m_isShowingSavingNotification;
        f64                         m_timeStartedNotifyingSave;
        i32                         m_savePromptCount;

        void                        updateSavingNotification();
        void                        registerSavegameErrorMenus();
        //Menu callbacks
        void                        onMenuItemAction (UIComponent* _UIComponent){}
        void                        onMenuItemPressAction(UIComponent* _UIComponent){}
        StringID                    onMenuPageAction(UIMenu * /*_menu*/, const StringID & _action, const StringID &_defaultAction);
        //
        static void                 loadSavePromptShowHideCallback(Adapter_Savegame::PromptType _prompt, bbool _show);

        f32     m_cutSceneDefaultUnskippableDurationFirstTime;
        CanInterruptCutSceneCallback_Fn  m_canInterruptCutSceneCallback;
        CutSceneAlreadySeenCallback_Fn m_cutSceneAlreadySeenCallback;
        TemplateClientHandler m_templateClientHandler;
    };
}

#endif // _ITF_GAMEINTERFACE_H_
