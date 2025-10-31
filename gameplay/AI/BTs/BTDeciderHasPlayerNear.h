#ifndef _ITF_BTDECIDERHASPLAYERNEAR_H_
#define _ITF_BTDECIDERHASPLAYERNEAR_H_

#ifndef _ITF_BTDECIDER_H_
#include "gameplay/ai/BTs/BTDecider.h"
#endif // _ITF_BTDECIDER_H_

namespace ITF
{

class BTDeciderHasPlayerNear : public BTDecider
{
    DECLARE_OBJECT_CHILD_RTTI(BTDeciderHasPlayerNear,BTDecider,3410105885);

public:

    BTDeciderHasPlayerNear();
    ~BTDeciderHasPlayerNear();

    virtual bbool                           decide( f32 _dt );

private:

    const class BTDeciderHasPlayerNear_Template*  getTemplate() const;

    ObjectRef                               checkRadius();
    ObjectRef                               checkDetectionArea( Actor* _detectionActor );

};

class BTDeciderHasPlayerNear_Template : public BTDecider_Template
{
    DECLARE_OBJECT_CHILD_RTTI(BTDeciderHasPlayerNear_Template,BTDecider_Template,15924324);
    DECLARE_SERIALIZE();
    DECLARE_BTNODE_TEMPLATE(BTDeciderHasPlayerNear);

public:

    BTDeciderHasPlayerNear_Template() : m_radius(2.f) {}
    ~BTDeciderHasPlayerNear_Template() {}

    const StringID&                         getFact() const { return m_fact; }
    const StringID&                         getDetectionArea() const { return m_detectionArea; }
    f32                                     getRadius() const { return m_radius; }

private:

    StringID                                m_fact;
    StringID                                m_detectionArea;
    f32                                     m_radius;
};

ITF_INLINE const BTDeciderHasPlayerNear_Template*  BTDeciderHasPlayerNear::getTemplate() const {return static_cast<const BTDeciderHasPlayerNear_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_BTDECIDERHASFACT_H_

