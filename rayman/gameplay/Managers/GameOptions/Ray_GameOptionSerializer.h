#ifndef _ITF_RAY_GAMEOPTIONSERIALIZER_H_
#define _ITF_RAY_GAMEOPTIONSERIALIZER_H_

#ifndef _ITF_RAY_GAMEOPTIONMANAGER_H_
#include "Ray_GameOptionManager.h"
#endif //_ITF_RAY_GAMEOPTIONMANAGER_H_

namespace ITF
{
    class Ray_GameOptionSerializer
    {
    public:
        static void serialize(const Ray_GameOptionManager& manager, Vector<u8>& buffer);
        static bbool deserialize(Ray_GameOptionManager& manager, const Vector<u8>& buffer);
        static u32 getSerializedSize(const Ray_GameOptionManager& manager);
        static u32 getVersion() { return GAMEOPTION_SERIALIZE_VERSION; }
        static u32 computeFormatSignatureCRC();

    private:
        static const u32 GAMEOPTION_SERIALIZE_VERSION = 1;
        static void writeU32(Vector<u8>& buffer, u32 value);
        static void writeI32(Vector<u8>& buffer, i32 value);
        static void writeF32(Vector<u8>& buffer, f32 value);
        static void writeU8(Vector<u8>& buffer, u8 value);
        static void writeBool(Vector<u8>& buffer, bbool value);
        static void writeStringID(Vector<u8>& buffer, const StringID& str);
        static u32 readU32(const u8*& ptr, const u8* end);
        static i32 readI32(const u8*& ptr, const u8* end);
        static f32 readF32(const u8*& ptr, const u8* end);
        static u8 readU8(const u8*& ptr, const u8* end);
        static bbool readBool(const u8*& ptr, const u8* end);
        static StringID readStringID(const u8*& ptr, const u8* end);
        static bbool checkBounds(const u8* ptr, const u8* end, u32 size);
    };
} // namespace ITF

#endif //_ITF_RAY_GAMEOPTIONSERIALIZER_H_
