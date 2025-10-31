#pragma once
#include <optional>
#include <variant>
#include <memory>
#include <type_traits>

#include "core/types.h"
#include "core/String8.h"
#include "core/container/extendedVector.h"

#include "shader_reflection.h"

namespace ITF::AGS
{
    using NumericType = SceShaderNumericClass;
    struct VectorDesc;
    struct MatrixDesc;
    struct ArrayDesc;
    struct StructDesc;
    struct PointerDesc;
    struct SamplerDesc;
    struct TextureDesc;
    struct BufferDesc;

    struct TypeDesc
    {
        using VariantType =
            std::variant<
                VectorDesc,
                MatrixDesc,
                ArrayDesc,
                StructDesc,
                PointerDesc,
                SamplerDesc,
                BufferDesc,
                TextureDesc>;
        using VariantTypePtr = std::unique_ptr<VariantType>;
        VariantTypePtr variantType; // Use a pointer here to allow the class declaration of declared but not yet defined classes.

        template <typename T>
        void setAsType(T&& oneOfVariant);

        static TypeDesc build(SceShaderMetadataSectionHandle md, SceShaderTypeHandle type, bool expandStructs);

    };

    static_assert(std::is_move_assignable_v<TypeDesc>);

    struct VectorDesc
    {
        NumericType type;
        u32 nbScalar = 1; // currently max 4
        static VectorDesc build(SceShaderMetadataSectionHandle md, SceShaderTypeHandle type);
    };

    static_assert(std::is_move_assignable_v<VectorDesc>);

    struct MatrixDesc
    {
        NumericType type;
        u32 nbCol = 1; // currently max 4
        u32 nbRow = 1; // currently max 4
        bool isRowMajor = false; // row or column major
        static MatrixDesc build(SceShaderMetadataSectionHandle md, SceShaderTypeHandle type);
    };

    static_assert(std::is_move_assignable_v<MatrixDesc>);

    struct ArrayDesc
    {
        Vector<i32> dimensionSizes{}; // usually 1 or 2 dimension
        TypeDesc type;
        static ArrayDesc build(SceShaderMetadataSectionHandle md, SceShaderTypeHandle type, bool expandStructs);
    };

    static_assert(std::is_move_assignable_v<ArrayDesc>);

    struct MemberDesc;

    struct StructDesc
    {
        String8 name;
        u32 size = 0; // in bytes
        Vector<MemberDesc> members{}; // name and type

        static StructDesc build(SceShaderMetadataSectionHandle md, SceShaderTypeHandle type, bool expandStructs);
    };

    static_assert(std::is_move_assignable_v<StructDesc>);

    struct MemberDesc
    {
        String8 name;
        TypeDesc type;
        u64 defaultValueSize = 0;
        i32 offset = 0; // in bytes
        i32 size = 0; // in bytes
        bool hasDefaultValue = false;
        bool isUsed = true; // if the member is used or not

        static MemberDesc build(SceShaderMetadataSectionHandle md, SceShaderMemberHandle m, bool expandStructs);
    };

    static_assert(std::is_move_assignable_v<MemberDesc>);

    struct PointerDesc
    {
        TypeDesc type;

        static PointerDesc build(SceShaderMetadataSectionHandle md, SceShaderTypeHandle type, bool expandStructs);
    };

    static_assert(std::is_move_assignable_v<PointerDesc>);

    struct SamplerDesc
    {
        static SamplerDesc build(SceShaderMetadataSectionHandle md, SceShaderTypeHandle type);
    };

    struct TextureDesc
    {
        SceShaderBufferClass bufferClass;
        TypeDesc type;
        bool isRW = false;
        static TextureDesc build(SceShaderMetadataSectionHandle md, SceShaderTypeHandle type, bool expandStructs);
    };

    static_assert(std::is_move_assignable_v<TextureDesc>);

    struct BufferDesc
    {
        SceShaderBufferClass bufferClass;
        TypeDesc type; // optional
        bool isRW = false;
        static BufferDesc build(SceShaderMetadataSectionHandle md, SceShaderTypeHandle type, bool expandStructs);
    };

    static_assert(std::is_move_assignable_v<BufferDesc>);

    struct SemanticDesc
    {
        String8 name;
        u32 index = 0;
        static SemanticDesc build(SceShaderMetadataSectionHandle md, SceShaderResourceHandle r);
    };

    struct ResourceDesc
    {
        String8 className;
        SceShaderResourceClass resourceClass = SceShaderUnknownResource;
        i32 apiSlot = 0;
        String8 name = "anonymous";
        std::optional<SemanticDesc> semantic;
        TypeDesc type; // optional

        static ResourceDesc build(SceShaderMetadataSectionHandle md, SceShaderResourceHandle r, bool expandStructs);
        static Vector<ResourceDesc> buildResourceCollection(SceShaderMetadataSectionHandle md, bool expandStructs);
    };

    static_assert(std::is_move_assignable_v<ResourceDesc>);
}
