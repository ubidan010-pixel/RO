
#ifndef _ITF_MUSICTREENODERANDOM_H_
#define _ITF_MUSICTREENODERANDOM_H_

#ifndef _ITF_BLENDTREENODEBLEND_H_
#include "engine/blendTree/BlendTreeNodeBlend.h"
#endif //_ITF_BLENDTREENODEBLEND_H_

#ifndef _ITF_BLENDTREENODE_H_
#include "engine/blendTree/BlendTreeNode.h"
#endif //_ITF_BLENDTREENODE_H_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_

#ifndef _ITF_MUSICTREERESULT_H_
#include "engine/blendTree/MusicTree/MusicTreeResult.h"
#endif //_ITF_MUSICTREERESULT_H_

namespace ITF
{

extern const char* MusicTreeNodeRandom_GetClassNameStatic();
extern const char* MusicTreeNodeRandom_Template_GetClassNameStatic();
class MusicTreeNodeRandom_Template;
class MusicTreeNodeRandom : public BlendTreeNodeBlend <MusicTreeResult>
{
    DECLARE_OBJECT_CHILD_RTTI(MusicTreeNodeRandom,BlendTreeNodeBlend<MusicTreeResult>,3145258087);

public:
	
	typedef FixedArray < BlendTreeNode <MusicTreeResult>*, 24 > NodeList;

    MusicTreeNodeRandom()
        : m_activeLeaf(U32_INVALID)
        , m_updateHasBeenCalledOnce( bfalse )
    {
    }

    virtual ~MusicTreeNodeRandom() { clear(); }

    virtual void                    getPlayingNodes( const InputContainer & _inputs, NodeList& _playingNodes );
    virtual void                    updateResult( f32 _deltaTime, const InputContainer & _inputs, BlendTreeResultBase* _result );
    virtual void                    onBecomeActive( const InputContainer& _inputs );
    virtual void                    onTransitionPlay();

    void                            fillArray();

protected:

    const class MusicTreeNodeRandom_Template * getTemplate() const ;

    void                            setActiveLeaf( const InputContainer& _inputs, u32 leafIndex );

    u32                             m_activeLeaf;

    bbool                           m_updateHasBeenCalledOnce;

    ITF_VECTOR<u32>                 m_indexesLeft;
};

////////////////////////////////////////////////////////////////////////////////////

class MusicTreeNodeRandom_Template : public BlendTreeNodeBlend_Template <MusicTreeResult>
{
    DECLARE_OBJECT_CHILD_RTTI(MusicTreeNodeRandom_Template,BlendTreeNodeBlend_Template<MusicTreeResult>,1249287778);
    DECLARE_SERIALIZE_VIRTUAL()
    DECLARE_MUSICTREENODE_TEMPLATE(MusicTreeNodeRandom)

public:

    MusicTreeNodeRandom_Template()
        : m_looping(btrue)
        , m_startingLeaf(U32_INVALID)
    {}
    ~MusicTreeNodeRandom_Template() {}

    bbool                       getLooping() const { return m_looping; }
    u32                         getStartingLeaf() const { return m_startingLeaf; }

private:

    bbool                       m_looping;
    u32                         m_startingLeaf;
};

ITF_INLINE const MusicTreeNodeRandom_Template * MusicTreeNodeRandom::getTemplate() const { 
        return static_cast<const MusicTreeNodeRandom_Template*>( this->m_template );
}


////////////////////////////////////////////////////////////////////////////////////

}

#endif // _ITF_MUSICTREENODERANDOM_H_

