
#ifndef _ITF_MUSICTREE_H_
#define _ITF_MUSICTREE_H_

#ifndef _ITF_BLENDTREE_H_
#include "engine/blendTree/BlendTree.h"
#endif //_ITF_BLENDTREE_H_

#ifndef _ITF_MUSICTREERESULT_H_
#include "engine/blendTree/MusicTree/MusicTreeResult.h"
#endif //_ITF_MUSICTREERESULT_H_

namespace ITF
{

class MusicTree : public BlendTree<MusicTreeResult>
{
    typedef BlendTree<MusicTreeResult> Super;

public:
    MusicTree() {}
    ~MusicTree() {}

    const class MusicTree_Template*    getTemplate() const;

private:

    virtual void                updateTransitions( f32 _dt, const InputContainer& _inputs ){}
};

typedef BlendTreeNode <MusicTreeResult> MusicTreeNode;

/////////////////////////////////////////////////////////////////////////////////

class MusicTree_Template : public BlendTreeTemplate <MusicTreeResult>
{
    DECLARE_OBJECT_CHILD_RTTI(MusicTree_Template,BlendTreeTemplate <MusicTreeResult>,890853302);
    DECLARE_SERIALIZE();

public:

    MusicTree_Template();
    virtual ~MusicTree_Template() {}

    ITF_INLINE const u32 getMetronomeType() const { return m_metronomeType; }

private:

    u32 m_metronomeType;
};

ITF_INLINE const MusicTree_Template* MusicTree::getTemplate() const
{
    return static_cast<const MusicTree_Template*>(m_template);
}

}

#endif // _ITF_MUSICTREE_H_

