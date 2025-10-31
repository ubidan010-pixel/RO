
#ifndef _ITF_MUSICTREENODEPLAYMUSIC_H_
#define _ITF_MUSICTREENODEPLAYMUSIC_H_

#ifndef _ITF_BLENDTREENODE_H_
#include "engine/blendTree/BlendTreeNode.h"
#endif //_ITF_BLENDTREENODE_H_

#ifndef _ITF_MUSICTREERESULT_H_
#include "engine/blendTree/MusicTree/MusicTreeResult.h"
#endif //_ITF_MUSICTREERESULT_H_



namespace ITF
{

class MusicTreeNodePlayMusic : public BlendTreeNode <MusicTreeResult>
{
    DECLARE_OBJECT_CHILD_RTTI(MusicTreeNodePlayMusic,BlendTreeNode<MusicTreeResult>,1142049534);
public:

    MusicTreeNodePlayMusic() {}
    virtual ~MusicTreeNodePlayMusic() {}

    virtual void            onLoadResource( BlendTreeLoadResourceBase* _resData );
    virtual void            updateResult( f32 _deltaTime, const InputContainer & _inputs, BlendTreeResultBase* _result );
    virtual void            onBecomeActive( const InputContainer& _inputs );
    virtual bbool           validate() const;

protected:

    ITF_INLINE const class MusicTreeNodePlayMusic_Template* getTemplate() const;

};

/////////////////////////////////////////////////////////////////////////////////

class MusicTreeNodePlayMusic_Template : public BlendTreeNodeTemplate <MusicTreeResult>
{
    DECLARE_OBJECT_CHILD_RTTI(MusicTreeNodePlayMusic_Template,BlendTreeNodeTemplate<MusicTreeResult>,1746451784);
    DECLARE_SERIALIZE_VIRTUAL()
    DECLARE_MUSICTREENODE_TEMPLATE(MusicTreeNodePlayMusic)

public:

    MusicTreeNodePlayMusic_Template() {}
    ~MusicTreeNodePlayMusic_Template() {}

    StringID                        m_musicName;
private:



};

ITF_INLINE const MusicTreeNodePlayMusic_Template* MusicTreeNodePlayMusic::getTemplate() const { return static_cast<const MusicTreeNodePlayMusic_Template*>(m_template); }

}

#endif // _ITF_MUSICTREENODEPLAYMUSIC_H_

