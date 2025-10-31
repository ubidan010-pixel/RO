#ifndef _ITF_ANIMATIONATLAS_H_
#define _ITF_ANIMATIONATLAS_H_

namespace ITF
{
    class AnimationAtlas
    {
        DECLARE_SERIALIZE();

    public:

        struct Key
        {
            DECLARE_SERIALIZE();

            Key(u32 _atlas, u32 _count)
                : m_atlas(_atlas)
                , m_count(_count)
            {
                // none
            }

            Key()
                : m_atlas(0)
                , m_count(0)
            {
                // none
            }

            u32 m_atlas;
            u32 m_count;
        };

        AnimationAtlas() {}
        ~AnimationAtlas() {}

        const SafeArray<Key>& getKeys() const { return m_sequence; }
        u32 getMaxFrame() const;
       
    //private:

        SafeArray<Key> m_sequence; 
    };


    class AnimationAtlasPlayer
    {
    public:

        AnimationAtlasPlayer()
            : m_anim(NULL)
            , m_curFrame(0)
            , m_maxFrame(0)
        {}
        ~AnimationAtlasPlayer() {}

        // init
        void init( const AnimationAtlas* _anim );

        void update();
        void setFrame(u32 _frame);
        u32  getCurAtlas();

    private:

        const AnimationAtlas* m_anim;
        u32 m_curFrame;
        u32 m_maxFrame;
    };
};

#endif //_ITF_ANIMATIONATLAS_H_