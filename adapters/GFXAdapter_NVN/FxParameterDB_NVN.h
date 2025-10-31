#pragma once

#include "core/math/matrix44/matrix44.h"

#include "core/container/extendedVector.h"

#include "core/StringID.h"

#include "NVN/Helpers.h"

#include "engine/AdaptersInterfaces/GFXAdapter.h"

// class to manage the shader parameters (aka uniforms) provided by the engine,
// and needed by techniques (vertex and fragment shaders)

namespace ITF
{
    class NVNSamplerPool;
    class NVNTexturePool;
    class NVNTexture;

    namespace NVN::ShaderMetaData
    {
        struct UniformBlockDesc;
        struct UniformDesc;
    }

    class FxParameterDB
    {
    public:

        // Template Handle class to differentiate between different types of parameters
        // This allow to avoid assignment of different handles.
        template <typename Tag>
        class Handle
        {
        public:
            Handle() = default;

            explicit Handle(i32 idx)
                : m_idx{ idx }
            {
            }

            // Parameters are save as void* in ITF_Shaders, so allow the conversion.
            // As we want to handle null test on these ITF_Shaders entries, the void* is -1,
            // and so nullptr is effectively invalid.
            explicit Handle(void* idxInVoidP) 
                : m_idx{ static_cast<i32>(reinterpret_cast<uPtr>(idxInVoidP)) - 1 }
            {
            }

            i32 getIdx() const { return m_idx; }
            void* getAsVoidP() const { return reinterpret_cast<void*>(m_idx + 1); }

            bool isValid() const { return m_idx != InvalidIndex; }

            enum : i32 { InvalidIndex = -1 };

        private:

            i32 m_idx = InvalidIndex;
        };

        using TexSamplerHdl = Handle<nvn::SeparateTextureHandle>;
        using Matrix4x4Hdl = Handle<Matrix44>;
        using FloatHdl = Handle<f32>;
        using IntHdl = Handle<i32>;
        using Float4Hdl = Handle<Float4>;
        struct Float4ArrayTag {};
        using Float4ArrayHdl = Handle<Float4ArrayTag>;

        // The name can be used to determine the right constant buffer to bind to
        void setName(const char* _name) { m_name = _name; m_nameID = _name; }
        const char * getName() const { return m_name.cStr(); }
        StringID getNameID() const { return m_nameID; }

        // shader parameters has to be registered at init
        TexSamplerHdl registerTexSampler(const char * _name, NVNSamplerPool & _samplerPool);
        Matrix4x4Hdl registerMatrix4x4(const char * _name);
        FloatHdl registerFloat(const char * _name);
        IntHdl registerInt(const char * _name);
        Float4Hdl registerFloat4(const char * _name);
        Float4ArrayHdl registerFloat4Array(const char * _name, u32 _size);

        // shaders 'binding' with the engine (at their loading)
        // need to get the handle from their uniforms names
        TexSamplerHdl findTexSampler(const char * _name) const;
        Matrix4x4Hdl findMatrix4x4(const char * _name) const;
        FloatHdl findFloat(const char * _name) const;
        IntHdl findInt(const char * _name) const;
        Float4Hdl findFloat4(const char * _name) const;
        Float4ArrayHdl findFloat4Array(const char * _name) const;

        // the engine set the shader parameters
        struct SamplerDesc
        {
        public:

            explicit SamplerDesc(NVNSamplerPool& _samplerPool)
            {
                updateSamplerHandle(_samplerPool);
            }

            SamplerDesc(const SamplerDesc&) = default;
            SamplerDesc & operator=(const SamplerDesc&) = default;

            SamplerDesc setWrapModeU(NVNSamplerPool& _samplerPool, GFX_TEXADRESSMODE _wrapModeU)
            {
                m_wrapModeU = _wrapModeU;
                updateSamplerHandle(_samplerPool);
                return *this;
            }

            GFX_TEXADRESSMODE getWrapModeU() const { return m_wrapModeU; }

            SamplerDesc setWrapModeV(NVNSamplerPool& _samplerPool, GFX_TEXADRESSMODE _wrapModeV)
            {
                m_wrapModeV = _wrapModeV;
                updateSamplerHandle(_samplerPool);
                return *this;
            }

            GFX_TEXADRESSMODE getWrapModeV() const { return m_wrapModeV; }

            SamplerDesc setFiltering(NVNSamplerPool& _samplerPool, bool _linearFiltering)
            {
                m_linearFiltering = _linearFiltering;
                updateSamplerHandle(_samplerPool);
                return *this;
            }

            void updateSamplerHandle(NVNSamplerPool& _samplerPool);

            nvn::SeparateSamplerHandle getSamplerHandle() const { return m_samplerHdl; }

        private:
            nvn::SeparateSamplerHandle m_samplerHdl{};
            GFX_TEXADRESSMODE m_wrapModeU = GFX_TEXADRESSMODE::GFXTADDRESS_WRAP;
            GFX_TEXADRESSMODE m_wrapModeV = GFX_TEXADRESSMODE::GFXTADDRESS_WRAP;
            bool m_linearFiltering = true;
        };

        struct TextureAndSamplerDesc
        {
            explicit TextureAndSamplerDesc(NVNSamplerPool& _samplerPool)
                : samplerDesc(_samplerPool)
            {
            }

            TextureAndSamplerDesc(const TextureAndSamplerDesc&) = default;
            TextureAndSamplerDesc & operator=(const TextureAndSamplerDesc&) = default;

            NVNTexture* texture = nullptr; // can be null if no texture is set. Used for debugging purpose
            nvn::SeparateTextureHandle textureHdl{};
            SamplerDesc samplerDesc;
        };

        void setTexture(TexSamplerHdl _hdl, NVNTexture* _tex);
        void setSampler(TexSamplerHdl _hdl, const SamplerDesc & _samplerDesc);

        void setMatrix4x4(Matrix4x4Hdl _hdl, const Matrix44 & _mat);
        void setFloat(FloatHdl _hdl, f32 _f);
        void setInt(FloatHdl _hdl, i32 _i);
        void setFloat4(Float4Hdl _hdl, const Float4& _v);
        void setFloat4Array(Float4ArrayHdl _hdl, const Float4 _v[], u32 _size); // size <= than the registered one
        template <int size>
        void setFloat4Array(Float4ArrayHdl _hdl, const Float4 _v[size]) // static array version
        {
            setFloat4Array(_hdl, _v, size);
        }

        // When a shader is selected, his parameters has to be gotten and sent to the GPU
        nvn::SeparateTextureHandle getTextureHdl(TexSamplerHdl _hdl) const;
        NVNTexture * getTexture(TexSamplerHdl _hdl) const;
        nvn::SeparateSamplerHandle getSamplerHdl(TexSamplerHdl _hdl) const;
        SamplerDesc getSamplerDesc(TexSamplerHdl _hdl) const;

        const Matrix44 & getMatrix4x4(Matrix4x4Hdl _hdl) const;
        f32 getFloat(FloatHdl _hdl) const;
        i32 getInt(IntHdl _hdl) const;
        const Float4& getFloat4(Float4Hdl _hdl) const;
        void getFloat4Array(Float4ArrayHdl _hdl, const Float4* & _float4Array, u32 & _nbFloat4) const;
        u32 getFloat4ArrayCapacity(Float4ArrayHdl _hdl) const;

    private:
        
        template<typename T>
        class ParamContainer
        {
        public:
            i32 add(const char * _name, const T & _initVal);
            enum : i32 { InvalidIndex = -1 };
            i32 find(const char* _name) const; // return InvalidIndex if not found
            const T & get(i32 _idx) const;
            T & get(i32 _idx);
            void set(i32 _idx, const T & _newVal);

        private:
            // names and params has to be same size (kept in 2 vectors to get better cache miss when accessing)
            using Names = Vector<String8> ;
            using Params = Vector<T> ;

            Names m_names;
            Params m_params;
        };

        ParamContainer<TextureAndSamplerDesc> m_texAndSamplers;
        ParamContainer<Matrix44> m_matrices;
        ParamContainer<f32> m_floats;
        ParamContainer<i32> m_ints;
        ParamContainer<Float4> m_float4s;
        using Float4Array = Vector<Float4>;
        ParamContainer<Float4Array> m_float4Arrays;

        String8 m_name;
        StringID m_nameID;
    };
}
