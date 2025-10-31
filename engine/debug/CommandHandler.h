#ifndef _ITF_COMMANDHANDLER_H_
#define _ITF_COMMANDHANDLER_H_

#if !defined(ITF_FINAL)


#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

namespace ITF
{
    #define CDHR_MAX_SUPPORTED_PLAYERS        8

    class Interface_CommandHandler
    {
    public:
        virtual bbool onKeyProcess (i32 _key, InputAdapter::PressStatus _keyStatus) = 0;
    };


    class CommandHandler : public Interface_InputListener, public TemplateSingleton<CommandHandler>, public Interface_CommandHandler
    {
        #define HIDE2D_DBG         1
        #define HIDE2D_2DACTORS    2
        #define HIDE2D_MASK        3


     public:
        CommandHandler();
        virtual ~CommandHandler();
        

        static  CommandHandler* getSingleton();

        // Interface_InputListener
        bbool   onMouseButton   (InputAdapter::MouseButton _but, InputAdapter::PressStatus _status);
        bbool   onMousePos      (i32 _x, i32 _y);
        bbool   onMouseWheel    (i32 _wheelValue, i32 _wheelDelta);
        bbool   onKey           (i32 _key, InputAdapter::PressStatus _keyStatus);
        
        // Interface_CommandHandler
        bbool   onKeyProcess    (i32 _key, InputAdapter::PressStatus _keyStatus);
        
        void    setEditor(Interface_CommandHandler* _editor) { m_editor = _editor; }

        void    resetPause();
        void    pushPause();
        void    popPause();

        void    loadLastCheckpoint(bbool _keepPlayersPos);
        void    reloadCurrentMap();

        void    update (double _elapsed);

    private:
        Vec3d   m_savedActorsPos[CDHR_MAX_SUPPORTED_PLAYERS];
        u32     m_restoreSavedActorsPosFrame;

        i32     m_hide2d;
        Interface_CommandHandler* m_editor;

        u32 safeFrameDefaultMode;
        u32 safeFrameUser;

    };

}
#endif //!ITF_FINAL

#endif //_ITF_COMMANDHANDLER_H_