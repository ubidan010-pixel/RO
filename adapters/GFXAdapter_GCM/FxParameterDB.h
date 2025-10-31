#if defined (ITF_PS3)

#ifndef ITF_FXPARAMETERDB_H_
#define ITF_FXPARAMETERDB_H_

#ifndef _ITF_MATRIX44_H_
#include "core/math/matrix44/matrix44.h"
#endif //_ITF_MATRIX44_H_

// class to manage the shader parameters (aka uniforms) provided by the engine,
// and needed by techniques (vertex and fragment shaders)

namespace ITF
{
    class FxParameterDB
    {
    public:
        typedef u32 TexSamplerHdl;
        typedef u32 Matrix4x4Hdl;
        typedef u32 FloatHdl;
        typedef u32 IntHdl;
        typedef u32 Float4Hdl;
        typedef u32 Float4ArrayHdl;

        enum 
        {
            InvalidHandle = 0
        };

        // todo : definitely choose mat library
        typedef ITF::Matrix44 Matrix44;
        typedef Float4 Vector4;

        enum Type
        {
            TYPE_GLOBAL = 0, // keep it 0 based as it's used for array indexing
            TYPE_COUNT = 1,
            TYPE_INVALID = 0xFFFFFFFFu
        };

        // return an ID to identify the type of parameter this DB provide (right now, only 1, the global)
        Type getType() const { return TYPE_GLOBAL; }

        // shader parameters has to be registered at init
        TexSamplerHdl registerTexSampler(const char * _name);
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
        void setTexture(TexSamplerHdl _hdl, void * _texture);
        void setMatrix4x4(Matrix4x4Hdl _hdl, const Matrix44 & _mat);
        void setFloat(FloatHdl _hdl, f32 _f);
        void setInt(FloatHdl _hdl, i32 _i);
        void setFloat4(Float4Hdl _hdl, const Vector4 & _v);
        void setFloat4Array(Float4ArrayHdl _hdl, const Vector4 _v[], u32 _size); // size <= than the registered one
        template <int size>
        void setFloat4Array(Float4ArrayHdl _hdl, const Vector4 _v[size]) // static array version
        {
            setFloat4Array(_hdl, _v, size);
        }
        
        // When a shader is selected, his parameters has to be gotten and sent to the GPU
        void * getTexture(TexSamplerHdl _hdl) const;
        const Matrix44 & getMatrix4x4(Matrix4x4Hdl _hdl) const;
        f32 getFloat(FloatHdl _hdl) const;
        i32 getInt(IntHdl _hdl) const;
        const Vector4 & getFloat4(Float4Hdl _hdl) const;
        void getFloat4Array(Float4ArrayHdl _hdl, const Vector4 * & _float4Array, u32 & _nbFloat4) const;

    private:
        
        template<typename T>
        class ParamContainer
        {
        public:
            typedef u32 Idx;
            Idx add(const char * _name, const T & _initVal);
            Idx find(const char * _name) const;
            const T & get(Idx _idx) const;
            T & get(Idx _idx);
            void set(Idx _idx, const T & _newVal);

        private:
            // names and params has to be same size (kept in 2 vectors to get better cache miss when accessing)
            typedef typename ITF_VECTOR<String8> Names;
            typedef typename ITF_VECTOR<T> Params;

            Names m_names;
            Params m_params;
        };

        ParamContainer<void *> m_texSamplers;
        ParamContainer<Matrix44> m_matrices;
        ParamContainer<f32> m_floats;
        ParamContainer<i32> m_ints;
        ParamContainer<Vector4> m_float4s;
        typedef SafeArray<Vector4> Float4Array;
        ParamContainer<Float4Array> m_float4Arrays;
    };
}

#endif // ITF_FXPARAMETERDB_H_

#endif // ITF_PS3