#include "precompiled_GFXAdapter_DX12.h"

#include "FxParameterDB_DX12.h"
#include "core/math/Vec2d.h"
#include "core/math/Vec3d.h"
#include "core/math/MathTools.h"
#include "Texture_DX12.h"

namespace ITF::DX12
{
    // shader parameters registered at init
    FxParameterDB::TexSamplerHdl FxParameterDB::registerTexSampler(const char * _name)
    {
        return TexSamplerHdl{ m_texAndSamplers.add(_name, {}) };
    }

    FxParameterDB::Matrix4x4Hdl FxParameterDB::registerMatrix4x4(const char * _name)
    {
        return Matrix4x4Hdl{ m_matrices.add(_name, GMatrixIdentity) };
    }

    FxParameterDB::FloatHdl FxParameterDB::registerFloat(const char * _name)
    {
        return FloatHdl{ m_floats.add(_name, 0.f) };
    }

    FxParameterDB::IntHdl FxParameterDB::registerInt(const char * _name)
    {
        return IntHdl{ m_ints.add(_name, 0) };
    }

    FxParameterDB::Float4Hdl FxParameterDB::registerFloat4(const char * _name)
    {
        return Float4Hdl{ m_float4s.add(_name, makeFloat4(0.f, 0.f, 0.f, 1.f)) };
    }

    FxParameterDB::Float4ArrayHdl FxParameterDB::registerFloat4Array(const char * _name, u32 _size)
    {
        // just reserve the size
        i32 idx = m_float4Arrays.add(_name, Float4Array());
        Float4Array & f4array = m_float4Arrays.get(idx);
        f4array.reserve(_size);
        return Float4ArrayHdl{ idx };
    }

    // get the shader parameter from a name
    FxParameterDB::TexSamplerHdl FxParameterDB::findTexSampler(const char * _name) const
    {
        return TexSamplerHdl{ m_texAndSamplers.find(_name) };
    }

    FxParameterDB::Matrix4x4Hdl FxParameterDB::findMatrix4x4(const char * _name) const
    {
        return Matrix4x4Hdl{ m_matrices.find(_name) };
    }

    FxParameterDB::FloatHdl FxParameterDB::findFloat(const char * _name) const
    {
        return FloatHdl{ m_floats.find(_name) };
    }

    FxParameterDB::IntHdl FxParameterDB::findInt(const char * _name) const
    {
        return IntHdl{ m_ints.find(_name) };
    }

    FxParameterDB::Float4Hdl FxParameterDB::findFloat4(const char * _name) const
    {
        return Float4Hdl{ m_float4s.find(_name) };
    }

    FxParameterDB::Float4ArrayHdl FxParameterDB::findFloat4Array(const char * _name) const
    {
        return Float4ArrayHdl{ m_float4Arrays.find(_name) };
    }


    // set the shader parameters in the DB

    void FxParameterDB::setTexture(TexSamplerHdl _hdl, Texture* _tex)
    {
        m_texAndSamplers.get(_hdl.getIdx()).texture = _tex;
    }

    void FxParameterDB::setSamplerMode(TexSamplerHdl _hdl, SamplerMode _samplerMode)
    {
        m_texAndSamplers.get(_hdl.getIdx()).samplerMode = _samplerMode;
    }

    void FxParameterDB::setMatrix4x4(Matrix4x4Hdl _hdl, const Matrix44 & _mat)
    {
        m_matrices.set(_hdl.getIdx(), _mat);
    }

    void FxParameterDB::setFloat(FloatHdl _hdl, f32 _f)
    {
        m_floats.set(_hdl.getIdx(), _f);
    }

    void FxParameterDB::setInt(FloatHdl _hdl, i32 _i)
    {
        m_ints.set(_hdl.getIdx(), _i);
    }

    void FxParameterDB::setFloat4(Float4Hdl _hdl, const Float4& _v)
    {
        m_float4s.set(_hdl.getIdx(), _v);
    }

    // size <= than the registered one
    void FxParameterDB::setFloat4Array(Float4ArrayHdl _hdl, const Float4 _v[], u32 _size)
    {
        Float4Array & f4array = m_float4Arrays.get(_hdl.getIdx());
        f4array.resize(_size);
        for(u32 i = 0; i<_size; ++i)
        {
            f4array[i] = _v[i];
        }
    }

    // get the shader parameters
    Texture * FxParameterDB::getTexture(TexSamplerHdl _hdl) const
    {
        return m_texAndSamplers.get(_hdl.getIdx()).texture;
    }

    FxParameterDB::SamplerMode FxParameterDB::getSamplerMode(TexSamplerHdl _hdl) const
    {
        return m_texAndSamplers.get(_hdl.getIdx()).samplerMode;
    }

    const Matrix44 & FxParameterDB::getMatrix4x4(Matrix4x4Hdl _hdl) const
    {
        return m_matrices.get(_hdl.getIdx());
    }

    f32 FxParameterDB::getFloat(FloatHdl _hdl) const
    {
        return m_floats.get(_hdl.getIdx());
    }

    i32 FxParameterDB::getInt(IntHdl _hdl) const
    {
        return m_ints.get(_hdl.getIdx());
    }

    const Float4 & FxParameterDB::getFloat4(Float4Hdl _hdl) const
    {
        return m_float4s.get(_hdl.getIdx());
    }

    void FxParameterDB::getFloat4Array(Float4ArrayHdl _hdl, const Float4* & _float4Array, u32 & _nbFloat4) const
    {
        const Float4Array & f4array = m_float4Arrays.get(_hdl.getIdx());
        if (f4array.size())
        {
            _float4Array = f4array.data();
        }
        else
        {
            _float4Array = nullptr;
        }
        _nbFloat4 = f4array.size();
    }

    u32 FxParameterDB::getFloat4ArrayCapacity(Float4ArrayHdl _hdl) const
    {
        const Float4Array& f4array = m_float4Arrays.get(_hdl.getIdx());
        return f4array.capacity();
    }

    // all type container common code
    template <typename T>
    i32 FxParameterDB::ParamContainer<T>::add(const char * _name, const T & _initVal)
    {
        ITF_ASSERT(m_names.size() == m_params.size());
        m_names.push_back(_name);
        m_params.push_back(_initVal);
        return i32(m_params.size()) - 1;
    }

    template <typename T>
    i32 FxParameterDB::ParamContainer<T>::find(const char * _name) const
    {
        ITF_ASSERT(m_names.size() == m_params.size());
        auto itFound = std::find(m_names.begin(), m_names.end(), _name);
        if (itFound != m_names.end())
        {
            return i32(itFound - m_names.begin());
        }
        return InvalidIndex;
    }

    template <typename T>
    const T & FxParameterDB::ParamContainer<T>::get(i32 _idx) const
    {
        ITF_ASSERT(m_names.size() == m_params.size());
        ITF_ASSERT(_idx < i32(m_names.size()));
        ITF_ASSERT(_idx >= 0);
        return m_params[_idx];
    }

    template <typename T>
    T & FxParameterDB::ParamContainer<T>::get(i32 _idx)
    {
        ITF_ASSERT(m_names.size() == m_params.size());
        ITF_ASSERT(_idx < i32(m_names.size()));
        ITF_ASSERT(_idx >= 0);
        return m_params[_idx];
    }
    
    template <typename T>
    void FxParameterDB::ParamContainer<T>::set(i32 _idx, const T & _newVal)
    {
        ITF_ASSERT(m_names.size() == m_params.size());
        ITF_ASSERT(_idx < i32(m_names.size()));
        ITF_ASSERT(_idx >= 0);
        m_params[_idx] = _newVal;
    }
}
