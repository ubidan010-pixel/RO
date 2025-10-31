#ifndef _ITF_GAMEPLAYFACTORYFILLER_H_
#define _ITF_GAMEPLAYFACTORYFILLER_H_

namespace ITF
{
    class GameplayFactoryFiller
    {
        public:
            static void fillFactories();

    private:
            static void fillEngineComponents();
            static void fillGameplayComponents();
            static void fillTemplateComponents();
            static void fillAIComponents();
            static void fillSequenceComponents();
            static void fillEventComponents();
            static void fillAnimTreeComponents();
            static void fillMusicTreeComponents();
            static void fillBtComponents();
            static void fillClassComponents();
    };
}

#endif // _ITF_GAMEPLAYFACTORYFILLER_H_