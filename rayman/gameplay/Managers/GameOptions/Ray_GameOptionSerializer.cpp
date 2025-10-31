#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_GAMEOPTIONSERIALIZER_H_
#include "Ray_GameOptionSerializer.h"
#endif

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif

#define SERIALIZER_LOG(format, ...) LOG("[GameOptionSerializer] - " format, ##__VA_ARGS__)

namespace ITF
{
    void Ray_GameOptionSerializer::serialize(const Ray_GameOptionManager& manager, Vector<u8>& buffer)
    {
        buffer.clear();
        writeU32(buffer, GAMEOPTION_SERIALIZE_VERSION);
        const Map<StringID, Ray_GameOption*>& options = manager.getAllOptions();
        u32 optionCount = options.size();
        writeU32(buffer, optionCount);

        SERIALIZER_LOG("Serializing %u options (version %u)", optionCount, GAMEOPTION_SERIALIZE_VERSION);
        for (Map<StringID, Ray_GameOption*>::const_iterator it = options.begin(); it != options.end(); ++it)
        {
            const Ray_GameOption* option = it->second;
            ITF_ASSERT(option);
            writeU8(buffer, (u8)option->getType());
            writeStringID(buffer, option->getId());
            switch (option->getType())
            {
            case Ray_GameOption::OptionType_Bool:
                {
                    const Ray_GameOptionBool* boolOpt = static_cast<const Ray_GameOptionBool*>(option);
                    writeBool(buffer, boolOpt->getValue());
                }
                break;

            case Ray_GameOption::OptionType_Int:
                {
                    const Ray_GameOptionInt* intOpt = static_cast<const Ray_GameOptionInt*>(option);
                    writeI32(buffer, intOpt->getValue());
                }
                break;

            case Ray_GameOption::OptionType_Float:
                {
                    const Ray_GameOptionFloat* floatOpt = static_cast<const Ray_GameOptionFloat*>(option);
                    writeF32(buffer, floatOpt->getValue());
                }
                break;

            case Ray_GameOption::OptionType_IntList:
                {
                    const Ray_GameOptionIntList* listOpt = static_cast<const Ray_GameOptionIntList*>(option);
                    writeI32(buffer, listOpt->getSelectedIndex());
                }
                break;

            case Ray_GameOption::OptionType_FloatList:
                {
                    const Ray_GameOptionFloatList* listOpt = static_cast<const Ray_GameOptionFloatList*>(option);
                    writeI32(buffer, listOpt->getSelectedIndex());
                }
                break;

            case Ray_GameOption::OptionType_StringList:
                {
                    const Ray_GameOptionStringList* listOpt = static_cast<const Ray_GameOptionStringList*>(option);
                    writeI32(buffer, listOpt->getSelectedIndex());
                }
                break;

            default:
                ITF_ASSERT_MSG(0, "Unknown option type: %d", option->getType());
                break;
            }
        }

        SERIALIZER_LOG("Serialization complete. Total size: %u bytes", buffer.size());
    }

    bbool Ray_GameOptionSerializer::deserialize(Ray_GameOptionManager& manager, const Vector<u8>& buffer)
    {
        if (buffer.empty())
        {
            SERIALIZER_LOG("Cannot deserialize: empty buffer");
            return bfalse;
        }

        const u8* ptr = &buffer[0];
        const u8* end = ptr + buffer.size();

        if (!checkBounds(ptr, end, sizeof(u32)))
        {
            SERIALIZER_LOG("Cannot deserialize: buffer too small for version (size=%u)", (u32)buffer.size());
            return bfalse;
        }

        u32 version = readU32(ptr, end);
        if (version != GAMEOPTION_SERIALIZE_VERSION)
        {
            SERIALIZER_LOG("Unsupported version: %u (expected %u)", version, GAMEOPTION_SERIALIZE_VERSION);
            return bfalse;
        }

        if (!checkBounds(ptr, end, sizeof(u32)))
        {
            SERIALIZER_LOG("Cannot deserialize: buffer too small for option count (remaining=%u)", (u32)(end - ptr));
            return bfalse;
        }

    u32 optionCount = readU32(ptr, end);
    SERIALIZER_LOG("Deserializing %u options (version %u)", optionCount, version);

        for (u32 i = 0; i < optionCount; ++i)
        {
            if (!checkBounds(ptr, end, sizeof(u8)))
            {
                SERIALIZER_LOG("Cannot deserialize option %u: buffer too small for type (remaining=%u)", i, (u32)(end - ptr));
                return bfalse;
            }

            u8 typeValue = readU8(ptr, end);
            Ray_GameOption::OptionType type = (Ray_GameOption::OptionType)typeValue;
            StringID id = readStringID(ptr, end);
            if (id == StringID())
            {
                SERIALIZER_LOG("Warning: option %u has invalid StringID; skipping payload for type %d", i, (int)type);
                switch (type)
                {
                case Ray_GameOption::OptionType_Bool:
                    if (!checkBounds(ptr, end, sizeof(u8))) return bfalse;
                    ptr += sizeof(u8);
                    break;
                case Ray_GameOption::OptionType_Int:
                case Ray_GameOption::OptionType_IntList:
                case Ray_GameOption::OptionType_FloatList:
                case Ray_GameOption::OptionType_StringList:
                    if (!checkBounds(ptr, end, sizeof(i32))) return bfalse;
                    ptr += sizeof(i32);
                    break;
                case Ray_GameOption::OptionType_Float:
                    if (!checkBounds(ptr, end, sizeof(f32))) return bfalse;
                    ptr += sizeof(f32);
                    break;
                default:
                    SERIALIZER_LOG("Unknown option type while skipping invalid ID: %d", type);
                    return bfalse;
                }
                continue;
            }

            if (!manager.hasOption(id))
            {
                SERIALIZER_LOG("Warning: Option '%s' not found in manager, skipping", id.getDebugString());

                switch (type)
                {
                case Ray_GameOption::OptionType_Bool:
                    if (!checkBounds(ptr, end, sizeof(u8))) return bfalse;
                    ptr += sizeof(u8);
                    break;
                case Ray_GameOption::OptionType_Int:
                case Ray_GameOption::OptionType_IntList:
                case Ray_GameOption::OptionType_FloatList:
                case Ray_GameOption::OptionType_StringList:
                    if (!checkBounds(ptr, end, sizeof(i32))) return bfalse;
                    ptr += sizeof(i32);
                    break;
                case Ray_GameOption::OptionType_Float:
                    if (!checkBounds(ptr, end, sizeof(f32))) return bfalse;
                    ptr += sizeof(f32);
                    break;
                default:
                    SERIALIZER_LOG("Unknown option type: %d", type);
                    return bfalse;
                }
                continue;
            }

            switch (type)
            {
            case Ray_GameOption::OptionType_Bool:
                {
                    if (!checkBounds(ptr, end, sizeof(u8)))
                    {
                        SERIALIZER_LOG("Cannot deserialize bool option '%s': buffer too small", id.getDebugString());
                        return bfalse;
                    }
                    bbool value = readBool(ptr, end);
                    manager.setBoolOption(id, value);
                }
                break;

            case Ray_GameOption::OptionType_Int:
                {
                    if (!checkBounds(ptr, end, sizeof(i32)))
                    {
                        SERIALIZER_LOG("Cannot deserialize int option '%s': buffer too small", id.getDebugString());
                        return bfalse;
                    }
                    i32 value = readI32(ptr, end);
                    manager.setIntOption(id, value);
                }
                break;

            case Ray_GameOption::OptionType_Float:
                {
                    if (!checkBounds(ptr, end, sizeof(f32)))
                    {
                        SERIALIZER_LOG("Cannot deserialize float option '%s': buffer too small", id.getDebugString());
                        return bfalse;
                    }
                    f32 value = readF32(ptr, end);
                    manager.setFloatOption(id, value);
                }
                break;

            case Ray_GameOption::OptionType_IntList:
            case Ray_GameOption::OptionType_FloatList:
            case Ray_GameOption::OptionType_StringList:
                {
                    if (!checkBounds(ptr, end, sizeof(i32)))
                    {
                        SERIALIZER_LOG("Cannot deserialize list option '%s': buffer too small", id.getDebugString());
                        return bfalse;
                    }
                    i32 index = readI32(ptr, end);
                    manager.setListOptionIndex(id, index);
                }
                break;

            default:
                SERIALIZER_LOG("Unknown option type %d for option '%s'", type, id.getDebugString());
                return bfalse;
            }
        }

        SERIALIZER_LOG("Deserialization complete");
        return btrue;
    }

    u32 Ray_GameOptionSerializer::computeFormatSignatureCRC()
    {
        u32 gameSaveCRC = 0;
        
        if (RAY_GAMEMANAGER)
        {
            gameSaveCRC = RAY_GAMEMANAGER->computeSaveCodeCRC();
        }
        
        SERIALIZER_LOG("computeFormatSignatureCRC: using game CRC = 0x%08x (unified with game saves)", gameSaveCRC);
        return gameSaveCRC;
    }

    u32 Ray_GameOptionSerializer::getSerializedSize(const Ray_GameOptionManager& manager)
    {
        u32 size = 0;
        size += sizeof(u32) * 2;
        const Map<StringID, Ray_GameOption*>& options = manager.getAllOptions();
        for (Map<StringID, Ray_GameOption*>::const_iterator it = options.begin(); it != options.end(); ++it)
        {
            const Ray_GameOption* option = it->second;
            size += sizeof(u8);
            size += sizeof(u32);
            switch (option->getType())
            {
            case Ray_GameOption::OptionType_Bool:
                size += sizeof(u8);
                break;
            case Ray_GameOption::OptionType_Int:
            case Ray_GameOption::OptionType_IntList:
            case Ray_GameOption::OptionType_FloatList:
            case Ray_GameOption::OptionType_StringList:
                size += sizeof(i32);
                break;
            case Ray_GameOption::OptionType_Float:
                size += sizeof(f32);
                break;
            }
        }

        return size;
    }

    void Ray_GameOptionSerializer::writeU32(Vector<u8>& buffer, u32 value)
    {
        buffer.push_back((u8)(value & 0xFF));
        buffer.push_back((u8)((value >> 8) & 0xFF));
        buffer.push_back((u8)((value >> 16) & 0xFF));
        buffer.push_back((u8)((value >> 24) & 0xFF));
    }

    void Ray_GameOptionSerializer::writeI32(Vector<u8>& buffer, i32 value)
    {
        writeU32(buffer, (u32)value);
    }

    void Ray_GameOptionSerializer::writeF32(Vector<u8>& buffer, f32 value)
    {
        u32 intValue;
        memcpy(&intValue, &value, sizeof(f32));
        writeU32(buffer, intValue);
    }

    void Ray_GameOptionSerializer::writeU8(Vector<u8>& buffer, u8 value)
    {
        buffer.push_back(value);
    }

    void Ray_GameOptionSerializer::writeBool(Vector<u8>& buffer, bbool value)
    {
        buffer.push_back(value ? 1 : 0);
    }

    void Ray_GameOptionSerializer::writeStringID(Vector<u8>& buffer, const StringID& str)
    {
        writeU32(buffer, str.GetValue());
    }

    u32 Ray_GameOptionSerializer::readU32(const u8*& ptr, const u8* end)
    {
        ITF_ASSERT(checkBounds(ptr, end, sizeof(u32)));
        u32 value = (u32)ptr[0] | ((u32)ptr[1] << 8) | ((u32)ptr[2] << 16) | ((u32)ptr[3] << 24);
        ptr += 4;
        return value;
    }

    i32 Ray_GameOptionSerializer::readI32(const u8*& ptr, const u8* end)
    {
        return (i32)readU32(ptr, end);
    }

    f32 Ray_GameOptionSerializer::readF32(const u8*& ptr, const u8* end)
    {
        u32 intValue = readU32(ptr, end);
        f32 floatValue;
        memcpy(&floatValue, &intValue, sizeof(f32));
        return floatValue;
    }

    u8 Ray_GameOptionSerializer::readU8(const u8*& ptr, const u8* end)
    {
        ITF_ASSERT(checkBounds(ptr, end, sizeof(u8)));
        return *ptr++;
    }

    bbool Ray_GameOptionSerializer::readBool(const u8*& ptr, const u8* end)
    {
        return readU8(ptr, end) != 0;
    }

    StringID Ray_GameOptionSerializer::readStringID(const u8*& ptr, const u8* end)
    {
        if (!checkBounds(ptr, end, sizeof(u32)))
            return StringID();
        u32 value = readU32(ptr, end);
        return StringID(value);
    }

    bbool Ray_GameOptionSerializer::checkBounds(const u8* ptr, const u8* end, u32 size)
    {
        return (ptr + size) <= end;
    }
} // namespace ITF
