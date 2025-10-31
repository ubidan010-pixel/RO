#ifndef _ITF_BTDECIDERHASFACT_H_
#define _ITF_BTDECIDERHASFACT_H_

#ifndef _ITF_BTDECIDER_H_
#include "gameplay/ai/BTs/BTDecider.h"
#endif // _ITF_BTDECIDER_H_

namespace ITF
{

class BTDeciderHasFact : public BTDecider
{
    DECLARE_OBJECT_CHILD_RTTI(BTDeciderHasFact,BTDecider,3633586099);

public:

    BTDeciderHasFact();
    ~BTDeciderHasFact();

    virtual bbool                           validate( Actor* _actor ) const;
    virtual bbool                           decide( f32 _dt );

private:

    const class BTDeciderHasFact_Template*  getTemplate() const;

};

class BTDeciderHasFact_Template : public BTDecider_Template
{
    DECLARE_OBJECT_CHILD_RTTI(BTDeciderHasFact_Template,BTDecider_Template,2356683879);
    DECLARE_SERIALIZE();
    DECLARE_BTNODE_TEMPLATE(BTDeciderHasFact);

public:

    BTDeciderHasFact_Template() {}
    ~BTDeciderHasFact_Template() {}

    const SafeArray<StringID>&                  getFactHave() const { return m_factHave; }
    const SafeArray<StringID>&                  getFactNotHave() const { return m_factNotHave; }

private:

    SafeArray <StringID>                        m_factHave;
    SafeArray <StringID>                        m_factNotHave;
};

ITF_INLINE const BTDeciderHasFact_Template*  BTDeciderHasFact::getTemplate() const {return static_cast<const BTDeciderHasFact_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_BTDECIDERHASFACT_H_

