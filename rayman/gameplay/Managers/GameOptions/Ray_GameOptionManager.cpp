#include "precompiled_gameplay_rayman.h"
#include "Ray_GameOptionManager.h"
#include "Ray_GameOptionSerializer.h"

#define GAMEOPTION_LOG(format, ...) {} //LOG("[GameOption] - " format, ##__VA_ARGS__)

namespace ITF
{
Ray_GameOption::Ray_GameOption(OptionType _type, const StringID& _id)
    : m_type(_type)
    , m_id(_id)
    , m_dirty(bfalse)
{
}

Ray_GameOption::~Ray_GameOption()
{
}

Ray_GameOptionBool::Ray_GameOptionBool()
    : Ray_GameOption(OptionType_Bool, StringID())
    , m_value(bfalse)
    , m_defaultValue(bfalse)
{
}

Ray_GameOptionBool::Ray_GameOptionBool(const StringID& _id, bbool _defaultValue)
    : Ray_GameOption(OptionType_Bool, _id)
    , m_value(_defaultValue)
    , m_defaultValue(_defaultValue)
{
}

Ray_GameOptionBool::~Ray_GameOptionBool()
{
}

void Ray_GameOptionBool::setValue(bbool _value)
{
    if (m_value != _value)
    {
        m_value = _value;
        setDirty();
    }
}

void Ray_GameOptionBool::reset()
{
    setValue(m_defaultValue);
}

Ray_GameOptionInt::Ray_GameOptionInt()
    : Ray_GameOption(OptionType_Int, StringID())
    , m_value(0)
    , m_defaultValue(0)
    , m_minValue(0)
    , m_maxValue(0)
{
}

Ray_GameOptionInt::Ray_GameOptionInt(const StringID& _id, i32 _defaultValue, i32 _minValue, i32 _maxValue)
    : Ray_GameOption(OptionType_Int, _id)
    , m_value(_defaultValue)
    , m_defaultValue(_defaultValue)
    , m_minValue(_minValue)
    , m_maxValue(_maxValue)
{
}

Ray_GameOptionInt::~Ray_GameOptionInt()
{
}

void Ray_GameOptionInt::setValue(i32 _value)
{
    i32 clampedValue = Clamp(_value, m_minValue, m_maxValue);
    if (m_value != clampedValue)
    {
        m_value = clampedValue;
        setDirty();
    }
}

void Ray_GameOptionInt::reset()
{
    setValue(m_defaultValue);
}

Ray_GameOptionFloat::Ray_GameOptionFloat()
    : Ray_GameOption(OptionType_Float, StringID())
    , m_value(0.0f)
    , m_defaultValue(0.0f)
    , m_minValue(0.0f)
    , m_maxValue(0.0f)
{
}

Ray_GameOptionFloat::Ray_GameOptionFloat(const StringID& _id, f32 _defaultValue, f32 _minValue, f32 _maxValue)
    : Ray_GameOption(OptionType_Float, _id)
    , m_value(_defaultValue)
    , m_defaultValue(_defaultValue)
    , m_minValue(_minValue)
    , m_maxValue(_maxValue)
{
}

Ray_GameOptionFloat::~Ray_GameOptionFloat()
{
}

void Ray_GameOptionFloat::setValue(f32 _value)
{
    f32 clampedValue = Clamp(_value, m_minValue, m_maxValue);
    if (m_value != clampedValue)
    {
        m_value = clampedValue;
        setDirty();
    }
}

void Ray_GameOptionFloat::reset()
{
    setValue(m_defaultValue);
}

Ray_GameOptionIntList::Ray_GameOptionIntList()
    : Ray_GameOption(OptionType_IntList, StringID())
    , m_options()
    , m_selectedIndex(0)
    , m_defaultIndex(0)
{
}

Ray_GameOptionIntList::Ray_GameOptionIntList(const StringID& _id, const Vector<i32>& _options, i32 _defaultIndex)
    : Ray_GameOption(OptionType_IntList, _id)
    , m_options(_options)
    , m_selectedIndex(_defaultIndex)
    , m_defaultIndex(_defaultIndex)
{
}

Ray_GameOptionIntList::~Ray_GameOptionIntList()
{
}

void Ray_GameOptionIntList::setSelectedIndex(i32 _index)
{
    if (_index >= 0 && _index < static_cast<i32>(m_options.size()))
    {
        if (m_selectedIndex != _index)
        {
            m_selectedIndex = _index;
            setDirty();
        }
    }
}

void Ray_GameOptionIntList::reset()
{
    setSelectedIndex(m_defaultIndex);
}

Ray_GameOptionFloatList::Ray_GameOptionFloatList()
    : Ray_GameOption(OptionType_FloatList, StringID())
    , m_options()
    , m_selectedIndex(0)
    , m_defaultIndex(0)
{
}

Ray_GameOptionFloatList::Ray_GameOptionFloatList(const StringID& _id, const Vector<f32>& _options, i32 _defaultIndex)
    : Ray_GameOption(OptionType_FloatList, _id)
    , m_options(_options)
    , m_selectedIndex(_defaultIndex)
    , m_defaultIndex(_defaultIndex)
{
}

Ray_GameOptionFloatList::~Ray_GameOptionFloatList()
{
}

void Ray_GameOptionFloatList::setSelectedIndex(i32 _index)
{
    if (_index >= 0 && _index < static_cast<i32>(m_options.size()))
    {
        if (m_selectedIndex != _index)
        {
            m_selectedIndex = _index;
            setDirty();
        }
    }
}

void Ray_GameOptionFloatList::reset()
{
    setSelectedIndex(m_defaultIndex);
}

Ray_GameOptionStringList::Ray_GameOptionStringList()
    : Ray_GameOption(OptionType_StringList, StringID())
    , m_options()
    , m_selectedIndex(0)
    , m_defaultIndex(0)
{
}

Ray_GameOptionStringList::Ray_GameOptionStringList(const StringID& _id, const Vector<StringID>& _options, i32 _defaultIndex)
    : Ray_GameOption(OptionType_StringList, _id)
    , m_options(_options)
    , m_selectedIndex(_defaultIndex)
    , m_defaultIndex(_defaultIndex)
{
}

Ray_GameOptionStringList::~Ray_GameOptionStringList()
{
}

void Ray_GameOptionStringList::setSelectedIndex(i32 _index)
{
    if (_index >= 0 && _index < static_cast<i32>(m_options.size()))
    {
        if (m_selectedIndex != _index)
        {
            m_selectedIndex = _index;
            setDirty();
        }
    }
}

void Ray_GameOptionStringList::reset()
{
    setSelectedIndex(m_defaultIndex);
}

Ray_GameOptionManager::Ray_GameOptionManager()
    : m_options()
{
}

Ray_GameOptionManager::~Ray_GameOptionManager()
{
    for (Map<StringID, Ray_GameOption*>::iterator it = m_options.begin(); it != m_options.end(); ++it)
    {
        delete it->second;
    }
    m_options.clear();
}

void Ray_GameOptionManager::init()
{
}

void Ray_GameOptionManager::reset()
{
    for (Map<StringID, Ray_GameOption*>::iterator it = m_options.begin(); it != m_options.end(); ++it)
    {
        it->second->reset();
    }
}

void Ray_GameOptionManager::registerOption(Ray_GameOption* _option)
{
    if (!_option)
        return;

    const StringID& optionId = _option->getId();
    Map<StringID, Ray_GameOption*>::iterator it = m_options.find(optionId);

    if (it != m_options.end())
    {
        GAMEOPTION_LOG("Replacing existing option: %s", optionId.getDebugString());
        delete it->second;
        it->second = _option;
    }
    else
    {
        GAMEOPTION_LOG("Registering new option: %s", optionId.getDebugString());
        m_options[optionId] = _option;
    }
}

void Ray_GameOptionManager::registerBoolOption(const StringID& _id, bbool _defaultValue)
{
    Ray_GameOptionBool* option = new Ray_GameOptionBool(_id, _defaultValue);
    registerOption(option);
}

void Ray_GameOptionManager::registerIntOption(const StringID& _id, i32 _defaultValue, i32 _minValue, i32 _maxValue)
{
    Ray_GameOptionInt* option = new Ray_GameOptionInt(_id, _defaultValue, _minValue, _maxValue);
    registerOption(option);
}

void Ray_GameOptionManager::registerFloatOption(const StringID& _id, f32 _defaultValue, f32 _minValue, f32 _maxValue)
{
    Ray_GameOptionFloat* option = new Ray_GameOptionFloat(_id, _defaultValue, _minValue, _maxValue);
    registerOption(option);
}

void Ray_GameOptionManager::registerIntListOption(const StringID& _id, const Vector<i32>& _options, i32 _defaultIndex)
{
    Ray_GameOptionIntList* option = new Ray_GameOptionIntList(_id, _options, _defaultIndex);
    registerOption(option);
}

void Ray_GameOptionManager::registerFloatListOption(const StringID& _id, const Vector<f32>& _options, i32 _defaultIndex)
{
    Ray_GameOptionFloatList* option = new Ray_GameOptionFloatList(_id, _options, _defaultIndex);
    registerOption(option);
}

void Ray_GameOptionManager::registerStringListOption(const StringID& _id, const Vector<StringID>& _options, i32 _defaultIndex)
{
    Ray_GameOptionStringList* option = new Ray_GameOptionStringList(_id, _options, _defaultIndex);
    registerOption(option);
}

Ray_GameOption* Ray_GameOptionManager::findOption(const StringID& _id)
{
    Map<StringID, Ray_GameOption*>::iterator it = m_options.find(_id);
    if (it != m_options.end())
    {
        return it->second;
    }
    return NULL;
}

const Ray_GameOption* Ray_GameOptionManager::findOption(const StringID& _id) const
{
    Map<StringID, Ray_GameOption*>::const_iterator it = m_options.find(_id);
    if (it != m_options.end())
    {
        return it->second;
    }
    return NULL;
}

bbool Ray_GameOptionManager::getBoolOption(const StringID& _id, bbool _defaultIfNotFound) const
{
    const Ray_GameOption* option = findOption(_id);
    if (option && option->getType() == Ray_GameOption::OptionType_Bool)
    {
        return static_cast<const Ray_GameOptionBool*>(option)->getValue();
    }
    return _defaultIfNotFound;
}

void Ray_GameOptionManager::setBoolOption(const StringID& _id, bbool _value)
{
    Ray_GameOption* option = findOption(_id);
    if (option && option->getType() == Ray_GameOption::OptionType_Bool)
    {
        static_cast<Ray_GameOptionBool*>(option)->setValue(_value);
    }
}

i32 Ray_GameOptionManager::getIntOption(const StringID& _id, i32 _defaultIfNotFound) const
{
    const Ray_GameOption* option = findOption(_id);
    if (option && option->getType() == Ray_GameOption::OptionType_Int)
    {
        return static_cast<const Ray_GameOptionInt*>(option)->getValue();
    }
    return _defaultIfNotFound;
}

void Ray_GameOptionManager::setIntOption(const StringID& _id, i32 _value)
{
    Ray_GameOption* option = findOption(_id);
    if (option && option->getType() == Ray_GameOption::OptionType_Int)
    {
        static_cast<Ray_GameOptionInt*>(option)->setValue(_value);
    }
}

f32 Ray_GameOptionManager::getFloatOption(const StringID& _id, f32 _defaultIfNotFound) const
{
    const Ray_GameOption* option = findOption(_id);
    if (option && option->getType() == Ray_GameOption::OptionType_Float)
    {
        return static_cast<const Ray_GameOptionFloat*>(option)->getValue();
    }
    return _defaultIfNotFound;
}

void Ray_GameOptionManager::setFloatOption(const StringID& _id, f32 _value)
{
    Ray_GameOption* option = findOption(_id);
    if (option && option->getType() == Ray_GameOption::OptionType_Float)
    {
        static_cast<Ray_GameOptionFloat*>(option)->setValue(_value);
    }
}

i32 Ray_GameOptionManager::getListOptionIndex(const StringID& _id, i32 _defaultIfNotFound) const
{
    const Ray_GameOption* option = findOption(_id);
    if (option)
    {
        switch (option->getType())
        {
        case Ray_GameOption::OptionType_IntList:
            return static_cast<const Ray_GameOptionIntList*>(option)->getSelectedIndex();
        case Ray_GameOption::OptionType_FloatList:
            return static_cast<const Ray_GameOptionFloatList*>(option)->getSelectedIndex();
        case Ray_GameOption::OptionType_StringList:
            return static_cast<const Ray_GameOptionStringList*>(option)->getSelectedIndex();
        default:
            break;
        }
    }
    return _defaultIfNotFound;
}

void Ray_GameOptionManager::setListOptionIndex(const StringID& _id, i32 _index)
{
    Ray_GameOption* option = findOption(_id);
    if (option)
    {
        switch (option->getType())
        {
        case Ray_GameOption::OptionType_IntList:
            static_cast<Ray_GameOptionIntList*>(option)->setSelectedIndex(_index);
            break;
        case Ray_GameOption::OptionType_FloatList:
            static_cast<Ray_GameOptionFloatList*>(option)->setSelectedIndex(_index);
            break;
        case Ray_GameOption::OptionType_StringList:
            static_cast<Ray_GameOptionStringList*>(option)->setSelectedIndex(_index);
            break;
        default:
            break;
        }
    }
}

i32 Ray_GameOptionManager::getIntListOptionValue(const StringID& _id, i32 _defaultIfNotFound) const
{
    const Ray_GameOption* option = findOption(_id);
    if (option && option->getType() == Ray_GameOption::OptionType_IntList)
    {
        return static_cast<const Ray_GameOptionIntList*>(option)->getValue();
    }
    return _defaultIfNotFound;
}

f32 Ray_GameOptionManager::getFloatListOptionValue(const StringID& _id, f32 _defaultIfNotFound) const
{
    const Ray_GameOption* option = findOption(_id);
    if (option && option->getType() == Ray_GameOption::OptionType_FloatList)
    {
        return static_cast<const Ray_GameOptionFloatList*>(option)->getValue();
    }
    return _defaultIfNotFound;
}

StringID Ray_GameOptionManager::getStringListOptionValue(const StringID& _id, const StringID& _defaultIfNotFound) const
{
    const Ray_GameOption* option = findOption(_id);
    if (option && option->getType() == Ray_GameOption::OptionType_StringList)
    {
        return static_cast<const Ray_GameOptionStringList*>(option)->getValue();
    }
    return _defaultIfNotFound;
}

bbool Ray_GameOptionManager::hasOption(const StringID& _id) const
{
    return findOption(_id) != NULL;
}

Ray_GameOption* Ray_GameOptionManager::getOption(const StringID& _id)
{
    return findOption(_id);
}

const Ray_GameOption* Ray_GameOptionManager::getOption(const StringID& _id) const
{
    return findOption(_id);
}

bbool Ray_GameOptionManager::hasAnyDirtyOptions() const
{
    for (Map<StringID, Ray_GameOption*>::const_iterator it = m_options.begin(); it != m_options.end(); ++it)
    {
        if (it->second->isDirty())
        {
            return btrue;
        }
    }
    return bfalse;
}

void Ray_GameOptionManager::clearAllDirtyFlags()
{
    for (Map<StringID, Ray_GameOption*>::iterator it = m_options.begin(); it != m_options.end(); ++it)
    {
        it->second->clearDirty();
    }
}

void Ray_GameOptionManager::serializeToBuffer(Vector<u8>& buffer) const
{
    Ray_GameOptionSerializer::serialize(*this, buffer);
}

bbool Ray_GameOptionManager::deserializeFromBuffer(const Vector<u8>& buffer)
{
    return Ray_GameOptionSerializer::deserialize(*this, buffer);
}

void Ray_GameOptionManager::removeOption(const StringID& _id)
{
    Map<StringID, Ray_GameOption*>::iterator it = m_options.find(_id);
    if (it != m_options.end())
    {
        GAMEOPTION_LOG("Removing option: %s", _id.getDebugString());
        delete it->second;
        m_options.erase(it);
    }
}

void Ray_GameOptionManager::removeAllOptions()
{
    GAMEOPTION_LOG("Removing all options (%u total)", m_options.size());
    for (Map<StringID, Ray_GameOption*>::iterator it = m_options.begin(); it != m_options.end(); ++it)
    {
        delete it->second;
    }
    m_options.clear();
}

bbool Ray_GameOptionManager::isOptionDirty(const StringID& _id) const
{
    const Ray_GameOption* option = findOption(_id);
    if (option)
    {
        return option->isDirty();
    }
    return bfalse;
}

void Ray_GameOptionManager::clearDirtyFlag(const StringID& _id)
{
    Ray_GameOption* option = findOption(_id);
    if (option)
    {
        option->clearDirty();
    }
}

void Ray_GameOptionManager::resetOption(const StringID& _id)
{
    Ray_GameOption* option = findOption(_id);
    if (option)
    {
        GAMEOPTION_LOG("Resetting option: %s", _id.getDebugString());
        option->reset();
    }
}

void Ray_GameOptionManager::resetOptionsMatching(const char* _prefix)
{
    if (!_prefix)
        return;

    u32 prefixLen = (u32)strlen(_prefix);
    u32 resetCount = 0;

    for (Map<StringID, Ray_GameOption*>::iterator it = m_options.begin(); it != m_options.end(); ++it)
    {
        const char* optionName = it->first.getDebugString();
        if (strncmp(optionName, _prefix, prefixLen) == 0)
        {
            it->second->reset();
            resetCount++;
        }
    }

    GAMEOPTION_LOG("Reset %u options matching prefix '%s'", resetCount, _prefix);
}

void Ray_GameOptionManager::logAllOptions() const
{
    GAMEOPTION_LOG("=== All Options (%u) ===", m_options.size());

    for (Map<StringID, Ray_GameOption*>::const_iterator it = m_options.begin(); it != m_options.end(); ++it)
    {
        const Ray_GameOption* option = it->second;
        const char* typeName = "Unknown";

        switch (option->getType())
        {
        case Ray_GameOption::OptionType_Bool:
            {
                const Ray_GameOptionBool* boolOpt = static_cast<const Ray_GameOptionBool*>(option);
                GAMEOPTION_LOG("  [Bool] %s = %s (default: %s)%s",
                    option->getId().getDebugString(),
                    boolOpt->getValue() ? "true" : "false",
                    boolOpt->getDefaultValue() ? "true" : "false",
                    option->isDirty() ? " [DIRTY]" : "");
            }
            break;

        case Ray_GameOption::OptionType_Int:
            {
                const Ray_GameOptionInt* intOpt = static_cast<const Ray_GameOptionInt*>(option);
                GAMEOPTION_LOG("  [Int] %s = %d (default: %d, range: %d-%d)%s",
                    option->getId().getDebugString(),
                    intOpt->getValue(),
                    intOpt->getDefaultValue(),
                    intOpt->getMinValue(),
                    intOpt->getMaxValue(),
                    option->isDirty() ? " [DIRTY]" : "");
            }
            break;

        case Ray_GameOption::OptionType_Float:
            {
                const Ray_GameOptionFloat* floatOpt = static_cast<const Ray_GameOptionFloat*>(option);
                GAMEOPTION_LOG("  [Float] %s = %.2f (default: %.2f, range: %.2f-%.2f)%s",
                    option->getId().getDebugString(),
                    floatOpt->getValue(),
                    floatOpt->getDefaultValue(),
                    floatOpt->getMinValue(),
                    floatOpt->getMaxValue(),
                    option->isDirty() ? " [DIRTY]" : "");
            }
            break;

        case Ray_GameOption::OptionType_IntList:
            {
                const Ray_GameOptionIntList* listOpt = static_cast<const Ray_GameOptionIntList*>(option);
                GAMEOPTION_LOG("  [IntList] %s = %d (index: %d/%u, default index: %d)%s",
                    option->getId().getDebugString(),
                    listOpt->getValue(),
                    listOpt->getSelectedIndex(),
                    listOpt->getOptions().size(),
                    listOpt->getDefaultIndex(),
                    option->isDirty() ? " [DIRTY]" : "");
            }
            break;

        case Ray_GameOption::OptionType_FloatList:
            {
                const Ray_GameOptionFloatList* listOpt = static_cast<const Ray_GameOptionFloatList*>(option);
                GAMEOPTION_LOG("  [FloatList] %s = %.2f (index: %d/%u, default index: %d)%s",
                    option->getId().getDebugString(),
                    listOpt->getValue(),
                    listOpt->getSelectedIndex(),
                    listOpt->getOptions().size(),
                    listOpt->getDefaultIndex(),
                    option->isDirty() ? " [DIRTY]" : "");
            }
            break;

        case Ray_GameOption::OptionType_StringList:
            {
                const Ray_GameOptionStringList* listOpt = static_cast<const Ray_GameOptionStringList*>(option);
                GAMEOPTION_LOG("  [StringList] %s = %s (index: %d/%u, default index: %d)%s",
                    option->getId().getDebugString(),
                    listOpt->getValue().getDebugString(),
                    listOpt->getSelectedIndex(),
                    listOpt->getOptions().size(),
                    listOpt->getDefaultIndex(),
                    option->isDirty() ? " [DIRTY]" : "");
            }
            break;
        }
    }

    GAMEOPTION_LOG("======================");
}

bbool Ray_GameOptionManager::validateOption(const StringID& _id) const
{
    const Ray_GameOption* option = findOption(_id);
    if (!option)
    {
        GAMEOPTION_LOG("Validation failed: Option '%s' not found", _id.getDebugString());
        return bfalse;
    }

    // Validate based on type
    switch (option->getType())
    {
    case Ray_GameOption::OptionType_Int:
        {
            const Ray_GameOptionInt* intOpt = static_cast<const Ray_GameOptionInt*>(option);
            i32 value = intOpt->getValue();
            if (value < intOpt->getMinValue() || value > intOpt->getMaxValue())
            {
                GAMEOPTION_LOG("Validation failed: Int option '%s' value %d out of range [%d, %d]",
                    _id.getDebugString(), value, intOpt->getMinValue(), intOpt->getMaxValue());
                return bfalse;
            }
        }
        break;

    case Ray_GameOption::OptionType_Float:
        {
            const Ray_GameOptionFloat* floatOpt = static_cast<const Ray_GameOptionFloat*>(option);
            f32 value = floatOpt->getValue();
            if (value < floatOpt->getMinValue() || value > floatOpt->getMaxValue())
            {
                GAMEOPTION_LOG("Validation failed: Float option '%s' value %.2f out of range [%.2f, %.2f]",
                    _id.getDebugString(), value, floatOpt->getMinValue(), floatOpt->getMaxValue());
                return bfalse;
            }
        }
        break;

    case Ray_GameOption::OptionType_IntList:
        {
            const Ray_GameOptionIntList* listOpt = static_cast<const Ray_GameOptionIntList*>(option);
            i32 index = listOpt->getSelectedIndex();
            if (index < 0 || index >= (i32)listOpt->getOptions().size())
            {
                GAMEOPTION_LOG("Validation failed: IntList option '%s' index %d out of range [0, %u)",
                    _id.getDebugString(), index, listOpt->getOptions().size());
                return bfalse;
            }
        }
        break;

    case Ray_GameOption::OptionType_FloatList:
        {
            const Ray_GameOptionFloatList* listOpt = static_cast<const Ray_GameOptionFloatList*>(option);
            i32 index = listOpt->getSelectedIndex();
            if (index < 0 || index >= (i32)listOpt->getOptions().size())
            {
                GAMEOPTION_LOG("Validation failed: FloatList option '%s' index %d out of range [0, %u)",
                    _id.getDebugString(), index, listOpt->getOptions().size());
                return bfalse;
            }
        }
        break;

    case Ray_GameOption::OptionType_StringList:
        {
            const Ray_GameOptionStringList* listOpt = static_cast<const Ray_GameOptionStringList*>(option);
            i32 index = listOpt->getSelectedIndex();
            if (index < 0 || index >= (i32)listOpt->getOptions().size())
            {
                GAMEOPTION_LOG("Validation failed: StringList option '%s' index %d out of range [0, %u)",
                    _id.getDebugString(), index, listOpt->getOptions().size());
                return bfalse;
            }
        }
        break;
    }

    return btrue;
}

void Ray_GameOptionManager::getAllOptionIds(Vector<StringID>& _outIds) const
{
    _outIds.clear();
    _outIds.reserve(m_options.size());

    for (Map<StringID, Ray_GameOption*>::const_iterator it = m_options.begin(); it != m_options.end(); ++it)
    {
        _outIds.push_back(it->first);
    }
}

void Ray_GameOptionManager::copyFrom(const Ray_GameOptionManager& _other)
{
    // Serialize from source
    Vector<u8> buffer;
    _other.serializeToBuffer(buffer);

    // Deserialize into this
    deserializeFromBuffer(buffer);

    GAMEOPTION_LOG("Copied %u options from another manager", _other.getOptionCount());
}

void Ray_GameOptionManager::copyOption(const StringID& _id, const Ray_GameOptionManager& _source)
{
    const Ray_GameOption* sourceOption = _source.findOption(_id);
    if (!sourceOption)
    {
        GAMEOPTION_LOG("Cannot copy option '%s': not found in source", _id.getDebugString());
        return;
    }

    Ray_GameOption* destOption = findOption(_id);
    if (!destOption)
    {
        GAMEOPTION_LOG("Cannot copy option '%s': not found in destination", _id.getDebugString());
        return;
    }

    if (sourceOption->getType() != destOption->getType())
    {
        GAMEOPTION_LOG("Cannot copy option '%s': type mismatch", _id.getDebugString());
        return;
    }

    // Copy value based on type
    switch (sourceOption->getType())
    {
    case Ray_GameOption::OptionType_Bool:
        {
            const Ray_GameOptionBool* src = static_cast<const Ray_GameOptionBool*>(sourceOption);
            Ray_GameOptionBool* dst = static_cast<Ray_GameOptionBool*>(destOption);
            dst->setValue(src->getValue());
        }
        break;

    case Ray_GameOption::OptionType_Int:
        {
            const Ray_GameOptionInt* src = static_cast<const Ray_GameOptionInt*>(sourceOption);
            Ray_GameOptionInt* dst = static_cast<Ray_GameOptionInt*>(destOption);
            dst->setValue(src->getValue());
        }
        break;

    case Ray_GameOption::OptionType_Float:
        {
            const Ray_GameOptionFloat* src = static_cast<const Ray_GameOptionFloat*>(sourceOption);
            Ray_GameOptionFloat* dst = static_cast<Ray_GameOptionFloat*>(destOption);
            dst->setValue(src->getValue());
        }
        break;

    case Ray_GameOption::OptionType_IntList:
        {
            const Ray_GameOptionIntList* src = static_cast<const Ray_GameOptionIntList*>(sourceOption);
            Ray_GameOptionIntList* dst = static_cast<Ray_GameOptionIntList*>(destOption);
            dst->setSelectedIndex(src->getSelectedIndex());
        }
        break;

    case Ray_GameOption::OptionType_FloatList:
        {
            const Ray_GameOptionFloatList* src = static_cast<const Ray_GameOptionFloatList*>(sourceOption);
            Ray_GameOptionFloatList* dst = static_cast<Ray_GameOptionFloatList*>(destOption);
            dst->setSelectedIndex(src->getSelectedIndex());
        }
        break;

    case Ray_GameOption::OptionType_StringList:
        {
            const Ray_GameOptionStringList* src = static_cast<const Ray_GameOptionStringList*>(sourceOption);
            Ray_GameOptionStringList* dst = static_cast<Ray_GameOptionStringList*>(destOption);
            dst->setSelectedIndex(src->getSelectedIndex());
        }
        break;
    }

    GAMEOPTION_LOG("Copied option '%s' from source", _id.getDebugString());
}

} // namespace ITF
