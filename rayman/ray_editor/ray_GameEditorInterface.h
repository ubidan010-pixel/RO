#ifndef _ITF_RAY_GAME_EDITOR_INTERFACE_H_
#define _ITF_RAY_GAME_EDITOR_INTERFACE_H_

#ifndef _ITF_GAME_EDITORINTERFACE_H_
    #include "editor/GameEditorInterface.h"
#endif //_ITF_GAME_EDITORINTERFACE_H_


namespace ITF
{

    class RayGameEditorInterface : public GameEditorInterface
    {
    public:
        virtual void registerPlugins() const;
    };
}

#endif // _ITF_RAY_GAME_EDITOR_INTERFACE_H_
