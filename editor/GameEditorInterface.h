#ifndef _ITF_GAME_EDITORINTERFACE_H_
#define _ITF_GAME_EDITORINTERFACE_H_


namespace ITF
{

    class GameEditorInterface
    {
    public:
        virtual ~GameEditorInterface() = default;
        virtual void registerPlugins() const = 0;
    };
}

#endif // _ITF_GAME_EDITORINTERFACE_H_
