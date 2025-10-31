#include "precompiled_GFXAdapter_GCM.h"

#ifndef _ITF_VEC2D_H_
#include "core/math/Vec2d.h"
#endif //_ITF_VEC2D_H_

#ifndef _ITF_VEC3D_H_
#include "core/math/Vec3d.h"
#endif //_ITF_VEC3D_H_

#ifndef _ITF_MATHTOOLS_H_
#include "core/math/MathTools.h"
#endif //_ITF_MATHTOOLS_H_

#include "FxParameterDB.h"


namespace ITF
{
    // shader parameters registered at init
    FxParameterDB::TexSamplerHdl FxParameterDB::registerTexSampler(const char * _name)
    {
        return m_texSamplers.add(_name, NULL);
    }

    FxParameterDB::Matrix4x4Hdl FxParameterDB::registerMatrix4x4(const char * _name)
    {
        return m_matrices.add(_name, GMatrixIdentity);
    }

    FxParameterDB::FloatHdl FxParameterDB::registerFloat(const char * _name)
    {
        return m_floats.add(_name, 0.f);
    }

    FxParameterDB::IntHdl FxParameterDB::registerInt(const char * _name)
    {
        return m_ints.add(_name, 0);
    }

    FxParameterDB::Float4Hdl FxParameterDB::registerFloat4(const char * _name)
    {
        return m_float4s.add(_name, makeFloat4(0.f, 0.f, 0.f, 1.f));
    }

    FxParameterDB::Float4ArrayHdl FxParameterDB::registerFloat4Array(const char * _name, u32 _size)
    {
        // just reserve the size
        u32 idx = m_float4Arrays.add(_name, Float4Array());
        Float4Array & f4array = m_float4Arrays.get(idx);
        f4array.reserve(_size);
        return idx;
    }

    // get the shader parameter from a name
    FxParameterDB::TexSamplerHdl FxParameterDB::findTexSampler(const char * _name) const
    {
        return m_texSamplers.find(_name);
    }

    FxParameterDB::Matrix4x4Hdl FxParameterDB::findMatrix4x4(const char * _name) const
    {
        return m_matrices.find(_name);
    }

    FxParameterDB::FloatHdl FxParameterDB::findFloat(const char * _name) const
    {
        return m_floats.find(_name);
    }

    FxParameterDB::IntHdl FxParameterDB::findInt(const char * _name) const
    {
        return m_ints.find(_name);
    }

    FxParameterDB::Float4Hdl FxParameterDB::findFloat4(const char * _name) const
    {
        return m_float4s.find(_name);
    }

    FxParameterDB::Float4ArrayHdl FxParameterDB::findFloat4Array(const char * _name) const
    {
        return m_float4Arrays.find(_name);
    }

    // set the shader parameter in the DB
    void FxParameterDB::setTexture(TexSamplerHdl _hdl, void * _texture)
    {
        return m_texSamplers.set(_hdl, _texture);
    }

    void FxParameterDB::setMatrix4x4(Matrix4x4Hdl _hdl, const Matrix44 & _mat)
    {
        return m_matrices.set(_hdl, _mat);
    }

    void FxParameterDB::setFloat(FloatHdl _hdl, f32 _f)
    {
        return m_floats.set(_hdl, _f);
    }

    void FxParameterDB::setInt(FloatHdl _hdl, i32 _i)
    {
        return m_ints.set(_hdl, _i);
    }

    void FxParameterDB::setFloat4(Float4Hdl _hdl, const Vector4 & _v)
    {
        return m_float4s.set(_hdl, _v);
    }

    // size <= than the registered one
    void FxParameterDB::setFloat4Array(Float4ArrayHdl _hdl, const Vector4 _v[], u32 _size)
    {
        Float4Array & f4array = m_float4Arrays.get(_hdl);
        f4array.resize(_size);
        for(u32 i = 0; i<_size; ++i)
        {
            f4array[i] = _v[i];
        }
    }

    // get the shader parameters
    void * FxParameterDB::getTexture(TexSamplerHdl _hdl) const
    {
        return m_texSamplers.get(_hdl);
    }

    const FxParameterDB::Matrix44 & FxParameterDB::getMatrix4x4(Matrix4x4Hdl _hdl) const
    {
        return m_matrices.get(_hdl);
    }

    f32 FxParameterDB::getFloat(FloatHdl _hdl) const
    {
        return m_floats.get(_hdl);
    }

    i32 FxParameterDB::getInt(IntHdl _hdl) const
    {
        return m_ints.get(_hdl);
    }

    const FxParameterDB::Vector4 & FxParameterDB::getFloat4(Float4Hdl _hdl) const
    {
        return m_float4s.get(_hdl);
    }

    void FxParameterDB::getFloat4Array(Float4ArrayHdl _hdl, const Vector4 * & _float4Array, u32 & _nbFloat4) const
    {
        const Float4Array & f4array = m_float4Arrays.get(_hdl);
        if (f4array.size())
        {
            _float4Array = &(f4array[0]);
        }
        else
        {
            _float4Array = NULL;
        }
        _nbFloat4 = f4array.size();
    }

    // all type container common code
    template <typename T>
    FxParameterDB::ParamContainer<T>::Idx
    FxParameterDB::ParamContainer<T>::add(const char * _name, const T & _initVal)
    {
        ITF_ASSERT(m_names.size() == m_params.size());
        m_names.push_back(_name);
        m_params.push_back(_initVal);
        return m_params.size();
    }

    template <typename T>
    FxParameterDB::ParamContainer<T>::Idx
    FxParameterDB::ParamContainer<T>::find(const char * _name) const
    {
        ITF_ASSERT(m_names.size() == m_params.size());
        Idx idx = 0;
        for(Names::const_iterator it = m_names.begin(), itEnd = m_names.end();
            it!=itEnd;
            ++it, idx++)
        {
            if (*it == _name)
            {
                return idx+1;
            }
        }
        return Idx(0);
    }

    template <typename T>
    const T & FxParameterDB::ParamContainer<T>::get(Idx _idx) const
    {
        ITF_ASSERT(m_names.size() == m_params.size());
        ITF_ASSERT(_idx <= m_names.size());
        ITF_ASSERT(_idx > 0);
        return m_params[_idx-1];
    }

    template <typename T>
    T & FxParameterDB::ParamContainer<T>::get(Idx _idx)
    {
        ITF_ASSERT(m_names.size() == m_params.size());
        ITF_ASSERT(_idx <= m_names.size());
        ITF_ASSERT(_idx > 0);
        return m_params[_idx-1];
    }
    
    template <typename T>
    void FxParameterDB::ParamContainer<T>::set(Idx _idx, const T & _newVal)
    {
        ITF_ASSERT(m_names.size() == m_params.size());
        ITF_ASSERT(_idx <= m_names.size());
        ITF_ASSERT(_idx > 0);
        m_params[_idx-1] = _newVal;
    }
}
