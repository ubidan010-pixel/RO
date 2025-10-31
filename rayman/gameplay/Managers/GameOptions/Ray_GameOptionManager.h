#ifndef _ITF_RAY_GAMEOPTIONMANAGER_H_
#define _ITF_RAY_GAMEOPTIONMANAGER_H_

#ifndef _ITF_STRINGID_H_
#include "core/StringID.h"
#endif //_ITF_STRINGID_H_

#ifndef ITF_MAP_H_
#include "core/container/Map.h"
#endif //ITF_MAP_H_

#ifndef ITF_VECTOR_H_
#include "core/container/Vector.h"
#endif //ITF_VECTOR_H_

namespace ITF
{

class Ray_GameOption
{
public:
    enum OptionType
    {
        OptionType_Bool,
        OptionType_Int,
        OptionType_Float,
        OptionType_IntList,
        OptionType_FloatList,
        OptionType_StringList
    };

    Ray_GameOption(OptionType _type, const StringID& _id);
    virtual ~Ray_GameOption();

    ITF_INLINE OptionType getType() const { return m_type; }
    ITF_INLINE const StringID& getId() const { return m_id; }
    ITF_INLINE bbool isDirty() const { return m_dirty; }
    ITF_INLINE void clearDirty() { m_dirty = bfalse; }
    ITF_INLINE void setDirty() { m_dirty = btrue; }

    virtual void reset() = 0;

protected:
    OptionType m_type;
    StringID m_id;
    bbool m_dirty;
};

class Ray_GameOptionBool : public Ray_GameOption
{
public:
    Ray_GameOptionBool();
    Ray_GameOptionBool(const StringID& _id, bbool _defaultValue);
    virtual ~Ray_GameOptionBool();

    ITF_INLINE bbool getValue() const { return m_value; }
    void setValue(bbool _value);

    ITF_INLINE bbool getDefaultValue() const { return m_defaultValue; }
    virtual void reset() override;

private:
    bbool m_value;
    bbool m_defaultValue;
};

class Ray_GameOptionInt : public Ray_GameOption
{
public:
    Ray_GameOptionInt();
    Ray_GameOptionInt(const StringID& _id, i32 _defaultValue, i32 _minValue, i32 _maxValue);
    virtual ~Ray_GameOptionInt();

    ITF_INLINE i32 getValue() const { return m_value; }
    void setValue(i32 _value);

    ITF_INLINE i32 getDefaultValue() const { return m_defaultValue; }
    ITF_INLINE i32 getMinValue() const { return m_minValue; }
    ITF_INLINE i32 getMaxValue() const { return m_maxValue; }
    virtual void reset() override;

private:
    i32 m_value;
    i32 m_defaultValue;
    i32 m_minValue;
    i32 m_maxValue;
};

class Ray_GameOptionFloat : public Ray_GameOption
{
public:
    Ray_GameOptionFloat();
    Ray_GameOptionFloat(const StringID& _id, f32 _defaultValue, f32 _minValue, f32 _maxValue);
    virtual ~Ray_GameOptionFloat();

    ITF_INLINE f32 getValue() const { return m_value; }
    void setValue(f32 _value);

    ITF_INLINE f32 getDefaultValue() const { return m_defaultValue; }
    ITF_INLINE f32 getMinValue() const { return m_minValue; }
    ITF_INLINE f32 getMaxValue() const { return m_maxValue; }
    virtual void reset() override;

private:
    f32 m_value;
    f32 m_defaultValue;
    f32 m_minValue;
    f32 m_maxValue;
};

class Ray_GameOptionIntList : public Ray_GameOption
{
public:
    Ray_GameOptionIntList();
    Ray_GameOptionIntList(const StringID& _id, const Vector<i32>& _options, i32 _defaultIndex);
    virtual ~Ray_GameOptionIntList();

    ITF_INLINE i32 getValue() const
    {
        ITF_ASSERT(m_selectedIndex >= 0 && m_selectedIndex < (i32)m_options.size());
        return m_options[m_selectedIndex];
    }
    ITF_INLINE i32 getSelectedIndex() const { return m_selectedIndex; }
    void setSelectedIndex(i32 _index);

    ITF_INLINE i32 getDefaultIndex() const { return m_defaultIndex; }
    ITF_INLINE const Vector<i32>& getOptions() const { return m_options; }
    virtual void reset() override;

private:
    Vector<i32> m_options;
    i32 m_selectedIndex;
    i32 m_defaultIndex;
};

class Ray_GameOptionFloatList : public Ray_GameOption
{
public:
    Ray_GameOptionFloatList();
    Ray_GameOptionFloatList(const StringID& _id, const Vector<f32>& _options, i32 _defaultIndex);
    virtual ~Ray_GameOptionFloatList();

    ITF_INLINE f32 getValue() const
    {
        ITF_ASSERT(m_selectedIndex >= 0 && m_selectedIndex < (i32)m_options.size());
        return m_options[m_selectedIndex];
    }
    ITF_INLINE i32 getSelectedIndex() const { return m_selectedIndex; }
    void setSelectedIndex(i32 _index);

    ITF_INLINE i32 getDefaultIndex() const { return m_defaultIndex; }
    ITF_INLINE const Vector<f32>& getOptions() const { return m_options; }
    virtual void reset() override;

private:
    Vector<f32> m_options;
    i32 m_selectedIndex;
    i32 m_defaultIndex;
};

class Ray_GameOptionStringList : public Ray_GameOption
{
public:
    Ray_GameOptionStringList();
    Ray_GameOptionStringList(const StringID& _id, const Vector<StringID>& _options, i32 _defaultIndex);
    virtual ~Ray_GameOptionStringList();

    ITF_INLINE const StringID& getValue() const
    {
        ITF_ASSERT(m_selectedIndex >= 0 && m_selectedIndex < (i32)m_options.size());
        return m_options[m_selectedIndex];
    }
    ITF_INLINE i32 getSelectedIndex() const { return m_selectedIndex; }
    void setSelectedIndex(i32 _index);

    ITF_INLINE i32 getDefaultIndex() const { return m_defaultIndex; }
    ITF_INLINE const Vector<StringID>& getOptions() const { return m_options; }
    virtual void reset() override;

private:
    Vector<StringID> m_options;
    i32 m_selectedIndex;
    i32 m_defaultIndex;
};

class Ray_GameOptionManager
{
public:
    Ray_GameOptionManager();
    ~Ray_GameOptionManager();

    void init();
    void reset();
    void registerBoolOption(const StringID& _id, bbool _defaultValue);
    void registerIntOption(const StringID& _id, i32 _defaultValue, i32 _minValue, i32 _maxValue);
    void registerFloatOption(const StringID& _id, f32 _defaultValue, f32 _minValue, f32 _maxValue);
    void registerIntListOption(const StringID& _id, const Vector<i32>& _options, i32 _defaultIndex);
    void registerFloatListOption(const StringID& _id, const Vector<f32>& _options, i32 _defaultIndex);
    void registerStringListOption(const StringID& _id, const Vector<StringID>& _options, i32 _defaultIndex);
    bbool getBoolOption(const StringID& _id, bbool _defaultIfNotFound = bfalse) const;
    void setBoolOption(const StringID& _id, bbool _value);
    i32 getIntOption(const StringID& _id, i32 _defaultIfNotFound = 0) const;
    void setIntOption(const StringID& _id, i32 _value);
    f32 getFloatOption(const StringID& _id, f32 _defaultIfNotFound = 0.0f) const;
    void setFloatOption(const StringID& _id, f32 _value);
    i32 getListOptionIndex(const StringID& _id, i32 _defaultIfNotFound = 0) const;
    void setListOptionIndex(const StringID& _id, i32 _index);
    i32 getIntListOptionValue(const StringID& _id, i32 _defaultIfNotFound = 0) const;
    f32 getFloatListOptionValue(const StringID& _id, f32 _defaultIfNotFound = 0.0f) const;
    StringID getStringListOptionValue(const StringID& _id, const StringID& _defaultIfNotFound = StringID()) const;
    bbool hasOption(const StringID& _id) const;
    Ray_GameOption* getOption(const StringID& _id);
    const Ray_GameOption* getOption(const StringID& _id) const;
    bbool hasAnyDirtyOptions() const;
    void clearAllDirtyFlags();
    const Map<StringID, Ray_GameOption*>& getAllOptions() const { return m_options; }
    u32 getOptionCount() const { return m_options.size(); }
    void serializeToBuffer(Vector<u8>& buffer) const;
    bbool deserializeFromBuffer(const Vector<u8>& buffer);
    void removeOption(const StringID& _id);
    void removeAllOptions();
    bbool isOptionDirty(const StringID& _id) const;
    void clearDirtyFlag(const StringID& _id);
    void resetOption(const StringID& _id);
    void resetOptionsMatching(const char* _prefix);
    void logAllOptions() const;
    bbool validateOption(const StringID& _id) const;
    void getAllOptionIds(Vector<StringID>& _outIds) const;
    void copyFrom(const Ray_GameOptionManager& _other);
    void copyOption(const StringID& _id, const Ray_GameOptionManager& _source);
private:
    Ray_GameOption* findOption(const StringID& _id);
    const Ray_GameOption* findOption(const StringID& _id) const;
    void registerOption(Ray_GameOption* _option);

    Map<StringID, Ray_GameOption*> m_options;
};

} // namespace ITF

#endif //_ITF_RAY_GAMEOPTIONMANAGER_H_
