#include "AgsLoaderInStruct_PS5.h"

namespace ITF::AGS
{
    // For weird reason this fct is missing in ags:
    const char* sceShaderGetTypeClassName(SceShaderTypeClass klass)
    {
        switch (klass)
        {
        case SceShaderVectorType: return "Vector";
        case SceShaderMatrixType: return "Matrix";
        case SceShaderArrayType: return "Array";
        case SceShaderStructType: return "Struct";
        case SceShaderPointerType: return "Pointer";
        case SceShaderTextureType: return "Texture";
        case SceShaderBufferType: return "Buffer";
        default:
            return "Unknown";
        }
    }


    StructDesc StructDesc::build(SceShaderMetadataSectionHandle md, SceShaderTypeHandle type, bool expandStructs)
    {
        StructDesc structDescResult;
        structDescResult.name = sceShaderGetTypeName(md, type);
        structDescResult.size = sceShaderGetStructSize(md, type);

        if (expandStructs)
        {
            for (SceShaderMemberHandle m = sceShaderGetFirstMember(md, type);
                nullptr != m;
                m = sceShaderGetNextMember(md, m))
            {
                structDescResult.members.emplace_back(MemberDesc::build(md, m, expandStructs));
            }
        }
        return structDescResult;
    }

    MemberDesc MemberDesc::build(SceShaderMetadataSectionHandle md, SceShaderMemberHandle m, bool expandStructs)
    {
        MemberDesc member{};
        member.name = sceShaderGetMemberName(md, m);
        member.type = TypeDesc::build(md, sceShaderGetMemberType(md, m), expandStructs);
        member.offset = sceShaderGetMemberOffset(md, m);
        member.size = sceShaderGetMemberSize(md, m);
        member.hasDefaultValue = sceShaderGetDefaultValue(md, m) != nullptr;
        if (member.hasDefaultValue)
            member.defaultValueSize = sceShaderGetDefaultValueSize(md, m);
        member.isUsed = sceShaderIsMemberUsed(md, m);
        return member;
    }

    VectorDesc VectorDesc::build(SceShaderMetadataSectionHandle md, SceShaderTypeHandle type)
    {
        VectorDesc vctDesc{};
        vctDesc.type = sceShaderGetNumericClass(md, type);
        vctDesc.nbScalar = sceShaderGetColumns(md, type);
        return vctDesc;
    }


    MatrixDesc MatrixDesc::build(SceShaderMetadataSectionHandle md, SceShaderTypeHandle type)
    {
        MatrixDesc matDesc{};
        matDesc.type = sceShaderGetNumericClass(md, type);
        matDesc.nbCol = sceShaderGetColumns(md, type);
        matDesc.nbRow = sceShaderGetRows(md, type);
        matDesc.isRowMajor = sceShaderIsRowMajor(md, type);
        return matDesc;
    }

    ArrayDesc ArrayDesc::build(SceShaderMetadataSectionHandle md, SceShaderTypeHandle type, bool expandStructs)
    {
        ArrayDesc arrDesc{};
        SceShaderTypeHandle elemTy = type;
        do
        {
            arrDesc.dimensionSizes.push_back(sceShaderGetArraySize(md, elemTy));
            elemTy = sceShaderGetArrayElement(md, elemTy);
        } while (SceShaderArrayType == sceShaderGetTypeClass(md, elemTy));
        arrDesc.type = TypeDesc::build(md, elemTy, expandStructs);
        return arrDesc;
    }

    BufferDesc BufferDesc::build(SceShaderMetadataSectionHandle md, SceShaderTypeHandle type, bool expandStructs)
    {
        BufferDesc bufDesc{};
        bufDesc.bufferClass = sceShaderGetBufferClass(md, type);
        bufDesc.isRW = sceShaderIsRwBuffer(md, type);
        if (SceShaderByteBuffer != bufDesc.bufferClass)
        {
            SceShaderTypeHandle const bufferElemType = sceShaderGetBufferElementType(md, type);
            if (nullptr != bufferElemType)
            {
                bufDesc.type = TypeDesc::build(md, bufferElemType, expandStructs);
            }
        }
        return bufDesc;
    }

    TextureDesc TextureDesc::build(SceShaderMetadataSectionHandle md, SceShaderTypeHandle type, bool expandStructs)
    {
        TextureDesc texDesc{};
        texDesc.bufferClass = sceShaderGetBufferClass(md, type);
        texDesc.isRW = sceShaderIsRwBuffer(md, type);
        if (SceShaderByteBuffer != texDesc.bufferClass)
        {
            SceShaderTypeHandle const bufferElemType = sceShaderGetBufferElementType(md, type);
            if (nullptr != bufferElemType)
            {
                texDesc.type = TypeDesc::build(md, bufferElemType, expandStructs);
            }
        }
        return texDesc;
    }

    PointerDesc PointerDesc::build(SceShaderMetadataSectionHandle md, SceShaderTypeHandle type, bool expandStructs)
    {
        PointerDesc ptrDesc{};
        ptrDesc.type = TypeDesc::build(md, sceShaderGetPointerElement(md, type), expandStructs);
        return ptrDesc;
    }

    SamplerDesc SamplerDesc::build(SceShaderMetadataSectionHandle md, SceShaderTypeHandle type)
    {
        return {};
    }

    SemanticDesc SemanticDesc::build(SceShaderMetadataSectionHandle md, SceShaderResourceHandle r)
    {
        SemanticDesc semaDesc{};
        semaDesc.name = sceShaderGetSemanticName(md, r);
        semaDesc.index = sceShaderGetSemanticIndex(md, r);
        return semaDesc;
    }

    ResourceDesc ResourceDesc::build(SceShaderMetadataSectionHandle md, SceShaderResourceHandle r, bool expandStructs)
    {
        ResourceDesc resDesc{};
        resDesc.resourceClass = sceShaderGetResourceClass(md, r);
        resDesc.className = sceShaderGetResourceClassName(resDesc.resourceClass);
        resDesc.apiSlot = sceShaderGetResourceApiSlot(md, r);
        char const* const name = sceShaderGetResourceName(md, r);
        if (name != nullptr) resDesc.name = name;

        if (SceShaderInputAttribute == resDesc.resourceClass || SceShaderOutputAttribute == resDesc.resourceClass)
        {
            resDesc.semantic = SemanticDesc::build(md, r);
        }
        SceShaderTypeHandle const type = sceShaderGetResourceType(md, r);
        if (nullptr != type)
        {
            resDesc.type = TypeDesc::build(md, type, true);
        }
        return resDesc;
    }

    template <typename T>
    void TypeDesc::setAsType(T&& oneOfVariant)
    {
        variantType = VariantTypePtr{ new (MemoryId::mId_GfxAdapter) VariantType{ std::forward<T>(oneOfVariant) } };
    }

    TypeDesc TypeDesc::build(
        SceShaderMetadataSectionHandle md,
        SceShaderTypeHandle type,
        bool expandStructs)
    {
        if (nullptr == type)
        {
            return {};
        }

        SceShaderTypeClass const klass = sceShaderGetTypeClass(md, type);

        TypeDesc res{};
        // res.type stay at null if no type

        switch (klass)
        {
        case SceShaderVectorType:
        {
            res.setAsType(VectorDesc::build(md, type));
        }
        break;
        case SceShaderMatrixType:
        {
            res.setAsType(MatrixDesc::build(md, type));
        }
        break;
        case SceShaderArrayType:
        {
            res.setAsType(ArrayDesc::build(md, type, expandStructs));
        }
        break;

        case SceShaderStructType:
            res.setAsType(StructDesc::build(md, type, expandStructs));
            break;

        case SceShaderPointerType:
        {
            res.setAsType(PointerDesc::build(md, type, expandStructs));
            break;
        }
        case SceShaderSamplerStateType:
        {
            res.setAsType(SamplerDesc::build(md, type));
            break;
        }
        case SceShaderTextureType:
        {
            res.setAsType(TextureDesc::build(md, type, expandStructs));
            break;
        }
        case SceShaderBufferType:
        {
            res.setAsType(BufferDesc::build(md, type, expandStructs));
            break;
        }
        default:
            // SceShaderFunctionShaderType and SceShaderUnknownType
            break;
        };

        return res;
    }

    Vector<ResourceDesc> ResourceDesc::buildResourceCollection(SceShaderMetadataSectionHandle md, bool expandStructs)
    {
        Vector<ResourceDesc> result;

        SceShaderResourceListHandle lst = sceShaderGetResourceList(md);

        for (SceShaderResourceHandle r = sceShaderGetFirstResource(md, lst);
            nullptr != r;
            r = sceShaderGetNextResource(md, r))
        {
            result.emplace_back(ResourceDesc::build(md, r, true));
        }
        return result;
    }
}
