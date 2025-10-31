#ifndef _ITF_BTDECIDERHASACTORSALIVE_H_
#define _ITF_BTDECIDERHASACTORSALIVE_H_

#ifndef _ITF_BTDECIDER_H_
#include "gameplay/ai/BTs/BTDecider.h"
#endif // _ITF_BTDECIDER_H_

namespace ITF
{

class BTDeciderHasActorsAlive : public BTDecider
{
    DECLARE_OBJECT_CHILD_RTTI(BTDeciderHasActorsAlive,BTDecider,1717840903);

public:

    BTDeciderHasActorsAlive();
    ~BTDeciderHasActorsAlive();

    virtual bbool                           validate( Actor* _actor ) const;
    virtual bbool                           decide( f32 _dt );

private:

    const class BTDeciderHasActorsAlive_Template*  getTemplate() const;

};

class BTDeciderHasActorsAlive_Template : public BTDecider_Template
{
    DECLARE_OBJECT_CHILD_RTTI(BTDeciderHasActorsAlive_Template,BTDecider_Template,59634142);
    DECLARE_SERIALIZE();
    DECLARE_BTNODE_TEMPLATE(BTDeciderHasActorsAlive);

public:

    BTDeciderHasActorsAlive_Template() : m_invert(bfalse), m_checkAll(bfalse) {}
    ~BTDeciderHasActorsAlive_Template() {}

    StringID                                getFactActor() const { return m_factActor; }
    bbool                                   getInvert() const { return m_invert; }
    bbool                                   getCheckAll() const { return m_checkAll; }

private:

    StringID                                m_factActor;
    bbool                                   m_invert;
    bbool                                   m_checkAll;
};

ITF_INLINE const BTDeciderHasActorsAlive_Template*  BTDeciderHasActorsAlive::getTemplate() const {return static_cast<const BTDeciderHasActorsAlive_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_BTDECIDERHASACTORSALIVE_H_

