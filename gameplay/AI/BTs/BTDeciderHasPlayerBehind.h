#ifndef _ITF_BTDECIDERHASPLAYERBEHIND_H_
#define _ITF_BTDECIDERHASPLAYERBEHIND_H_

#ifndef _ITF_BTDECIDER_H_
#include "gameplay/ai/BTs/BTDecider.h"
#endif // _ITF_BTDECIDER_H_

namespace ITF
{

class BTDeciderHasPlayerBehind : public BTDecider
{
    DECLARE_OBJECT_CHILD_RTTI(BTDeciderHasPlayerBehind,BTDecider,3825940088);

public:

    BTDeciderHasPlayerBehind();
    ~BTDeciderHasPlayerBehind();

    virtual bbool                           validate( Actor* _actor ) const;
    virtual bbool                           decide( f32 _dt );

private:

    const class BTDeciderHasPlayerBehind_Template*  getTemplate() const;

    void                                    getLevelDir( Vec2d& _levelDir ) const;

};

class BTDeciderHasPlayerBehind_Template : public BTDecider_Template
{
    DECLARE_OBJECT_CHILD_RTTI(BTDeciderHasPlayerBehind_Template,BTDecider_Template,2308490063);
    DECLARE_SERIALIZE();
    DECLARE_BTNODE_TEMPLATE(BTDeciderHasPlayerBehind);

public:

    BTDeciderHasPlayerBehind_Template() : m_radius(1.f), m_margin(0.f), m_invert(bfalse), m_checkAll(bfalse) {}
    ~BTDeciderHasPlayerBehind_Template() {}

    f32                                     getRadius() const { return m_radius; }
    f32                                     getMargin() const { return m_margin; }
    bbool                                   getInvert() const { return m_invert; }
    bbool                                   getCheckAll() const { return m_checkAll; }
    StringID                                getFactDir() const { return m_factDir; }

private:

    f32                                     m_radius;
    f32                                     m_margin;
    bbool                                   m_invert;
    bbool                                   m_checkAll;
    StringID                                m_factDir;
};

ITF_INLINE const BTDeciderHasPlayerBehind_Template*  BTDeciderHasPlayerBehind::getTemplate() const {return static_cast<const BTDeciderHasPlayerBehind_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_BTDECIDERHASPLAYERBEHIND_H_

