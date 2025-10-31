
#ifndef _ITF_MUSICTREENODESEQUENCE_H_
#define _ITF_MUSICTREENODESEQUENCE_H_

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

extern const char* MusicTreeNodeSequence_GetClassNameStatic();
extern const char* MusicTreeNodeSequence_Template_GetClassNameStatic();
class MusicTreeNodeSequence_Template;
class MusicTreeNodeSequence : public BlendTreeNodeBlend <MusicTreeResult>
{
    DECLARE_OBJECT_CHILD_RTTI(MusicTreeNodeSequence,BlendTreeNodeBlend<MusicTreeResult>,3185466625);

public:
	
	typedef FixedArray < BlendTreeNode <MusicTreeResult>*, 24 > NodeList;

    MusicTreeNodeSequence()
        : m_activeLeaf(U32_INVALID)
        , m_updateHasBeenCalledOnce( bfalse )
    {
    }

    virtual ~MusicTreeNodeSequence() { clear(); }

    virtual void                    getPlayingNodes( const InputContainer & _inputs, NodeList& _playingNodes );
    virtual void                    updateResult( f32 _deltaTime, const InputContainer & _inputs, BlendTreeResultBase* _result );
    virtual void                    onBecomeActive( const InputContainer& _inputs );
    virtual void                    onTransitionPlay();

protected:

    const class MusicTreeNodeSequence_Template * getTemplate() const ;

    void                            setActiveLeaf( const InputContainer& _inputs, u32 leafIndex );

    u32                             m_activeLeaf;

    bbool                           m_updateHasBeenCalledOnce;
};

////////////////////////////////////////////////////////////////////////////////////

class MusicTreeNodeSequence_Template : public BlendTreeNodeBlend_Template <MusicTreeResult>
{
    DECLARE_OBJECT_CHILD_RTTI(MusicTreeNodeSequence_Template,BlendTreeNodeBlend_Template<MusicTreeResult>,2816407103);
    DECLARE_SERIALIZE_VIRTUAL()
    DECLARE_MUSICTREENODE_TEMPLATE(MusicTreeNodeSequence)

public:

    MusicTreeNodeSequence_Template()
        : m_startingLeaf(U32_INVALID)
        , m_looping(bfalse)
        , m_useIntro(bfalse)
    {}
    ~MusicTreeNodeSequence_Template() {}

    u32                         getStartingLeaf() const { return m_startingLeaf; }
    bbool                       getLooping() const { return m_looping; }
    bbool                       getUseIntro() const { return m_useIntro; }

private:

    u32                         m_startingLeaf;
    bbool                       m_useIntro;
    bbool                       m_looping;
};

ITF_INLINE const MusicTreeNodeSequence_Template * MusicTreeNodeSequence::getTemplate() const { 
        return static_cast<const MusicTreeNodeSequence_Template*>( this->m_template );
}


////////////////////////////////////////////////////////////////////////////////////

}

#endif // _ITF_MUSICTREENODESEQUENCE_H_

