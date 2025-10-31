#include "precompiled_engine.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_

namespace ITF
{

BEGIN_SERIALIZATION(InputDesc)

    SERIALIZE_MEMBER("name",m_id);
    SERIALIZE_ENUM_BEGIN("varType",m_type);
    SERIALIZE_ENUM_VAR(InputType_F32);
    SERIALIZE_ENUM_VAR(InputType_U32);
    SERIALIZE_ENUM_END();

END_SERIALIZATION()

BEGIN_SERIALIZATION(Input)

    SERIALIZE_MEMBER("name",m_id);
    SERIALIZE_ENUM_BEGIN("varType",m_type);
        SERIALIZE_ENUM_VAR(InputType_F32);
        SERIALIZE_ENUM_VAR(InputType_U32);
    SERIALIZE_ENUM_END();

END_SERIALIZATION()

//======================================================================================================
//======================================================================================================

BEGIN_SERIALIZATION(CriteriaDesc)

    SERIALIZE_MEMBER("name",m_name);
    SERIALIZE_MEMBER("eval",m_eval);
    SERIALIZE_MEMBER("value",m_value);
    SERIALIZE_MEMBER("isOR",m_or);

END_SERIALIZATION()

//======================================================================================================
//======================================================================================================

Criteria* Criteria::createCriteria( const CriteriaDesc& _desc, InputContainer* _inputContainer )
{
    Criteria * crit = NULL;

    if ( !_desc.m_name.isValid() )
    {
        ITF_FATAL_ERROR("missing name");
    }

    u32 inputIndex = U32_INVALID;
    u32 numInputs = _inputContainer->size();

    for ( u32 i = 0; i < numInputs; i++ )
    {
        Input& input = (*_inputContainer)[i];

        if ( input.getId() == _desc.m_name )
        {
            inputIndex = i;
            break;
        }
    }

    if ( inputIndex == U32_INVALID )
    {
        ITF_ERROR("Input '%s' was not defined", _desc.m_name.getDebugString());
        return NULL;
    }


    Input& inputRef = (*_inputContainer)[inputIndex];
    Input compare;

    compare.setType(inputRef.getType());

    if (inputRef.getType() == InputType_F32)
        compare.setValue( _desc.m_value );
    else
        compare.setValue( static_cast<u32>(_desc.m_value) );

    if ( _desc.m_eval == ITF_GET_STRINGID_CRC(<,2335045996) )
    {
        crit = newAlloc(mId_GameplayInputCriteria, Criteria(inputRef,Criteria::LessThan, compare, _desc.m_or ));
    }
    else if (_desc.m_eval == ITF_GET_STRINGID_CRC(<=,354964481) )
    {
        crit = newAlloc(mId_GameplayInputCriteria,  Criteria(inputRef,Criteria::LessThanOrEquals, compare, _desc.m_or ));
    }
    else if (_desc.m_eval == ITF_GET_STRINGID_CRC(>,2360266278) )
    {
        crit = newAlloc(mId_GameplayInputCriteria,  Criteria(inputRef,Criteria::GreaterThan, compare, _desc.m_or ));
    }
    else if (_desc.m_eval == ITF_GET_STRINGID_CRC(>=,1292350271) )
    {
        crit = newAlloc(mId_GameplayInputCriteria,  Criteria(inputRef,Criteria::GreaterThanOrEquals, compare, _desc.m_or ));
    }
    else if (_desc.m_eval == ITF_GET_STRINGID_CRC(==,361566127) )
    {
        crit = newAlloc(mId_GameplayInputCriteria,  Criteria(inputRef,Criteria::Equals, compare, _desc.m_or ));
    }
    else if (_desc.m_eval == ITF_GET_STRINGID_CRC(!=,263827075) )
    {
        crit = newAlloc(mId_GameplayInputCriteria,  Criteria(inputRef,Criteria::NotEquals, compare, _desc.m_or ));
    }
    else
    {
        // criteria not supported
        ITF_ASSERT(0);
    }

    return crit;
}

bbool Criteria::matchCriteria( const CriteriaArray& _criterias )
{
    bbool match = _criterias.empty();

    for (ITF_VECTOR<Criteria*>::const_iterator it = _criterias.begin(); it != _criterias.end(); ++it)
    {
        const Criteria* criteria = *it;

        match = criteria->isEqual();

        if ( !match )
        {
            if ( !criteria->isOr() )
            {
                break;
            }
        }
        else
        {
            if ( criteria->isOr() )
            {
                break;
            }
        }
    }

    return match;
}

//======================================================================================================
//======================================================================================================
#define F_MOD 0x1
#define F_ABS 0x2
#define F_ADD 0x4

BEGIN_SERIALIZATION(ProceduralInputData)

    bbool bmod = m_flags & F_MOD;
    bbool babs = m_flags & F_ABS;
    bbool badd = m_flags & F_ADD;

    SERIALIZE_MEMBER("input",m_input);
    SERIALIZE_MEMBER("min",m_minInput);
    SERIALIZE_MEMBER("max",m_maxInput);
    SERIALIZE_MEMBER("minValue",m_minVal);
    SERIALIZE_MEMBER("maxValue",m_maxVal);
    SERIALIZE_MEMBER("mod",bmod);
    SERIALIZE_MEMBER("abs",babs);
    SERIALIZE_MEMBER("add",badd);

    m_flags = 0;
    if(bmod)
        m_flags |= F_MOD;
    if(babs)
        m_flags |= F_ABS;
    if(badd)
        m_flags |= F_ADD;

END_SERIALIZATION()

ProceduralInputData::ProceduralInputData()
: m_inputIndex(U32_INVALID)
, m_minInput(0.f)
, m_maxInput(1.f)
, m_minVal(0.f)
, m_maxVal(1.f)
, m_flags(0)
{
}

f32 ProceduralInputData::getValue( f32 _currentValue, f32 _dt, const InputContainer& _inputs ) const
{
    if ( m_inputIndex == U32_INVALID )
    {
        u32 numInputs = _inputs.size();

        for ( u32 i = 0; i < numInputs; i++ )
        {
            const Input& input = _inputs[i];

            if ( input.getId() == m_input )
            {
                m_inputIndex = i;
                break;
            }
        }
    }

    if ( m_inputIndex == U32_INVALID )
    {
        ITF_WARNING_CATEGORY(GPP,NULL, 0, "input not defined: %s", m_input.getDebugString());
        return 0.f;
    }
    else
    {
        f32 inputValueF = 0.f;
        const Input& input = _inputs[m_inputIndex];

        if ( input.getType() == InputType_F32 )
        {
            input.getValue(inputValueF);
        }
        else
        {
            u32 inputValueU = 0;
            input.getValue(inputValueU);
            inputValueF = static_cast<f32>(inputValueU);
        }

        ITF_ASSERT(m_minInput!=m_maxInput);

        if ( m_minInput != m_maxInput )
        {
            if ( m_flags & F_ABS )
            {
                inputValueF = f32_Abs(inputValueF);
            }

            f32 maxInput = m_maxInput - m_minInput;

            inputValueF -= m_minInput;

            if ( (m_flags & F_MOD) && inputValueF > maxInput )
            {
                inputValueF = f32_Modulo(inputValueF,maxInput);
            }

            f32 t = inputValueF / maxInput;

            t = Clamp(t,0.f,1.f);

            f32 val = Interpolate(m_minVal,m_maxVal,t);

            if ( m_flags & F_ADD )
            {
                return _currentValue + (val*_dt);
            }
            else
            {
                return val;
            }
        }
        else
        {
            return m_minVal;
        }
    }
}

}
