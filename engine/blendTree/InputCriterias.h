#ifndef _ITF_INPUTCRITERIAS_H_
#define _ITF_INPUTCRITERIAS_H_

namespace ITF
{

class IUpdateAnimInputCriteria
{
public:

    virtual void updateAnimInput() = 0;
};

class IUpdateSoundInputCriteria
{
public:

    virtual void updateSoundInput() = 0;
};

class IUpdateAfterFxInputCriteria
{
public:

    virtual void updateAfterFxInput() = 0;
};

class IUpdateFxInputCriteria
{
public:

    virtual void updateFxInput() = 0;
};

enum InputType
{
    InputType_F32,
    InputType_U32,
    InputType_COUNT,
    ENUM_FORCE_SIZE_32(InputType)
};

class InputDesc
{
public:
    DECLARE_SERIALIZE()

    ITF_INLINE InputType getType() const {return m_type;}
    ITF_INLINE const StringID& getId() const { return m_id; }

#ifdef ITF_DEBUG_STRINGID
    ITF_INLINE const char* getName() const { return m_id.getDebugString(); }
#endif

private:

    InputType m_type;
    StringID m_id;
};

class Input
{
public:
    DECLARE_SERIALIZE()

    ITF_INLINE void setValue(f32 _value) { f32Value = _value;}
    ITF_INLINE void setValue(u32 _value) { u32Value = _value;}
    ITF_INLINE void setValue(bbool _value) { u32Value = static_cast<u32>(_value);}
    ITF_INLINE InputType getType() const {return m_type;}
    ITF_INLINE void setName( const char* _name ) { m_id = _name; }
#ifdef ITF_DEBUG_STRINGID
    ITF_INLINE const char* getName() const { return m_id.getDebugString(); }
#endif
    ITF_INLINE StringID getId() const { return m_id; }
    ITF_INLINE void setId( const StringID& _id ) { m_id = _id; }

    ITF_INLINE void setType(InputType _type)
    {
        m_type = _type;

        if (getType() == InputType_F32)
        {
            setValue(0.f);
        }
        else
        {
            setValue(static_cast<u32>(0));
        }
    }

    ITF_INLINE bool equals(const Input & _other) const
    {
        ITF_ASSERT(getType() == _other.getType());

        if (getType() == InputType_F32)
        {
            return f32Value == _other.f32Value;
        }
        else
        {
            return u32Value == _other.u32Value;
        }
    }
    ITF_INLINE bool lessThan(const Input & _other) const
    {
        ITF_ASSERT(getType() == _other.getType());

        if (getType() == InputType_F32)
        {
            return f32Value < _other.f32Value;
        }
        else
        {
            return u32Value < _other.u32Value;
        }
    }
    ITF_INLINE bool greaterThan(const Input & _other) const
    {
        ITF_ASSERT(getType() == _other.getType());

        if (getType() == InputType_F32)
        {
            return f32Value > _other.f32Value;
        }
        else
        {
            return u32Value > _other.u32Value;
        }
    }
    ITF_INLINE bool lessThanOrEquals(const Input & _other) const
    {
        ITF_ASSERT(getType() == _other.getType());

        if (getType() == InputType_F32)
        {
            return f32Value <= _other.f32Value;
        }
        else
        {
            return u32Value <= _other.u32Value;
        }
    }
    ITF_INLINE bool greaterThanOrEquals(const Input & _other) const
    {
        ITF_ASSERT(getType() == _other.getType());

        if (getType() == InputType_F32)
        {
            return f32Value >= _other.f32Value;
        }
        else
        {
            return u32Value >= _other.u32Value;
        }
    }

    ITF_INLINE void getValue( u32& _val ) const
    {
        ITF_ASSERT(getType() == InputType_U32);

        if (getType() == InputType_U32)
        {
            _val = u32Value;
        }
    }

    ITF_INLINE void getValue( bbool& _val ) const
    {
        ITF_ASSERT(getType() == InputType_U32);

        if (getType() == InputType_U32)
        {
            _val = (u32Value == 0) ? bfalse : btrue;
        }
    }

    ITF_INLINE void getValue( f32& _val ) const
    {
        ITF_ASSERT(getType() == InputType_F32);

        if (getType() == InputType_F32)
        {
            _val = f32Value;
        }
    }

private:
    InputType m_type;
    StringID m_id;
    union
    {
        f32 f32Value;
        u32 u32Value;
    };
};

typedef SafeArray <Input> InputContainer;

struct CriteriaDesc
{
    CriteriaDesc() : m_value(0.f), m_or(bfalse) {}

    DECLARE_SERIALIZE()
    StringID    m_name;
    StringID    m_eval;
    f32         m_value;
    bbool       m_or;
};

class Criteria
{
public:
    enum Type
    {
        LessThan,
        LessThanOrEquals,
        GreaterThan,
        GreaterThanOrEquals,
        Equals,
        NotEquals,
        ENUM_FORCE_SIZE_32(0)
    };
    Criteria(const Input & _value, Type _type, const Input & _compare, bbool _or ) : m_value(_value), m_type(_type), m_compare(_compare), m_or(_or) {}
    bool isEqual() const
    {
        switch(m_type)
        {
        case LessThan:
            return m_value.lessThan(m_compare);
        case LessThanOrEquals:
            return m_value.lessThanOrEquals(m_compare);
        case GreaterThan:
            return m_value.greaterThan(m_compare);
        case GreaterThanOrEquals:
            return m_value.greaterThanOrEquals(m_compare);
        case Equals:
            return m_value.equals(m_compare);
        case NotEquals:
            return !m_value.equals(m_compare);
        default:
            ITF_ASSERT(0);
            return false;
        }
    }

    bbool isOr() const { return m_or; }

    static Criteria* createCriteria( const CriteriaDesc& _desc, InputContainer* _inputContainer );
    static bbool matchCriteria( const ITF_VECTOR<Criteria*>& _criterias );

private:
    Criteria&  operator=(const Criteria& _other){ ITF_ASSERT(0);return *this;}
    const Input & m_value;
    const Input m_compare;
    const Type  m_type;
    bbool       m_or;
};

typedef ITF_VECTOR<Criteria*> CriteriaArray;
typedef ITF_VECTOR<CriteriaDesc> CriteriaDescArray;

class ProceduralInputData
{
    DECLARE_SERIALIZE()

public:
    ProceduralInputData();

    bbool               isValid() const { return m_input.isValid(); }
    const StringID&     getInput() const { return m_input; }
    f32                 getValue( f32 _currentValue, f32 _dt, const InputContainer& _inputs ) const;
    f32                 getMinValue() const { return m_minVal; }
    f32                 getMaxValue() const { return m_maxVal; }

private:

    StringID            m_input;
    mutable u32         m_inputIndex;
    f32                 m_minInput;
    f32                 m_maxInput;
    f32                 m_minVal;
    f32                 m_maxVal;
    u8                  m_flags;
};

}

#endif // _ITF_INPUTCRITERIAS_H_
