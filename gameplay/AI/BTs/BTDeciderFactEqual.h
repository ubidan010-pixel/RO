#ifndef _ITF_BTDECIDERFACTEQUAL_H_
#define _ITF_BTDECIDERFACTEQUAL_H_

#ifndef _ITF_BTDECIDER_H_
#include "gameplay/ai/BTs/BTDecider.h"
#endif // _ITF_BTDECIDER_H_

#ifndef _ITF_FACT_H_
#include "gameplay/ai/BTs/Fact.h"
#endif // _ITF_FACT_H_

namespace ITF
{

class BTDeciderFactEqual : public BTDecider
{
    DECLARE_OBJECT_CHILD_RTTI(BTDeciderFactEqual,BTDecider,2055185368);

public:

    BTDeciderFactEqual();
    ~BTDeciderFactEqual();

    virtual bbool                               validate( Actor* _actor ) const;
    virtual bbool                               decide( f32 _dt );

private:

    const class BTDeciderFactEqual_Template*    getTemplate() const;

};

class BTDeciderFactEqual_Template : public BTDecider_Template
{
    DECLARE_OBJECT_CHILD_RTTI(BTDeciderFactEqual_Template,BTDecider_Template,3776094096);
    DECLARE_SERIALIZE();
    DECLARE_BTNODE_TEMPLATE(BTDeciderFactEqual);

public:

    BTDeciderFactEqual_Template() : m_type(EValueType_Unknown) {}
    ~BTDeciderFactEqual_Template() {}

    const StringID&                             getFact() const { return m_fact; }
    const String8&                              getValue() const { return m_value; }
    EValueType                                  getType() const { return m_type; }

private:

    StringID                                    m_fact;
    String8                                     m_value;
    EValueType                                  m_type;
};

ITF_INLINE const BTDeciderFactEqual_Template*  BTDeciderFactEqual::getTemplate() const {return static_cast<const BTDeciderFactEqual_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_BTDECIDERFACTEQUAL_H_

