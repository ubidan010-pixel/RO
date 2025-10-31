#include "precompiled_engine.h"

#ifndef ITF_WII

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_GFXADAPTER_SHADERMANAGER_H_
#include "engine/AdaptersInterfaces/GFXAdapter_ShaderManager.h"
#endif //_ITF_GFXADAPTER_SHADERMANAGER_H_

#ifdef ITF_IPAD
#include <algorithm>
#endif

namespace ITF
{
    
    // --------------------------------------------------------------------------------------

    ITF_shader::ITF_shader( const String& _name  )
        : m_name(_name)
    {
    }

    ITF_shader::~ITF_shader()
    {
        release();
    }

    void ITF_shader::release()
    {
		GFX_ADAPTER->releaseShader(this);

		mp_Vs.clear();
		mp_Ps.clear();
	}

    bbool ITF_shader::loadShaderGroup(    )
    {
        return GFX_ADAPTER->loadShader(this);
    }

    void ITF_shader::setVectorArray(u32 _h, const GFX_Vector4* _va, u32 _count   )
    {
        GFX_ADAPTER->fx_setVectorArray(mp_effect, m_VectorArrayHandle[_h], _va, _count);
    }

    void ITF_shader::setVector(u32 _h, const GFX_Vector4* _v   )   
    {
        GFX_ADAPTER->fx_setVector(mp_effect, m_VecHandle[_h], _v);
    }

    void ITF_shader::setFloat(u32 _h, float _f  ) 
    {
        GFX_ADAPTER->fx_setFloat(mp_effect, m_FloatHandle[_h], _f); 
    }

    void ITF_shader::setInt(u32 _h, i32 _i  ) 
    {
        GFX_ADAPTER->fx_setInt(mp_effect, m_IntHandle[_h], _i); 
    }

    void ITF_shader::commitChange()
    {
        GFX_ADAPTER->fx_commitChange(mp_effect);
    }


    // --------------------------------------------------------------------------------------

    GFXAdapter_shaderManager::GFXAdapter_shaderManager(   )
    {
        m_shaderBuffer.m_curBuffFloatPos[0] = 0;
        m_shaderBuffer.m_curBuffFloatPos[1] = 0;
        for (u32 i = 0; i < MAX_NB_CONSTANTBUFFER; i++)
            m_shaderBuffer.m_numberVector4ConstToFlush[i] = 0;
    }

    GFXAdapter_shaderManager::~GFXAdapter_shaderManager(   )
    {
        destroyAll();
    }

    void GFXAdapter_shaderManager::loadShaderGroup( const String& _name )
    {
        ITF_shader* shader = newAlloc(mId_GfxAdapter,ITF_shader(_name));
        if (shader->loadShaderGroup())
            addShaderGroup(shader);
        else
            SF_DEL(shader);
    }

    void GFXAdapter_shaderManager::createShaderGroup( const String& _name )
    {
        ITF_shader* shader = newAlloc(mId_GfxAdapter,ITF_shader(_name));
            addShaderGroup(shader);
    }

    void GFXAdapter_shaderManager::removeShaderGroup( ITF_shader* _shader  )
    {
        ITF_VECTOR<ITF_shader*>::iterator i= std::find(mp_shaderList.begin(),
            mp_shaderList.end(),
            _shader);  

        if (mp_shaderList.end() != i)
        {
            (*i)->release();
            delete (*i);
            mp_shaderList.erase(i);
        }
    }

    void GFXAdapter_shaderManager::addShaderGroup( ITF_shader* _shader  )
    {
        mp_shaderList.push_back(_shader);
    }

    ITF_shader* GFXAdapter_shaderManager::getShaderByIndex( u32 _id )
    {
        ITF_ASSERT(_id < mp_shaderList.size());
        return mp_shaderList[_id];
    }

    i32 GFXAdapter_shaderManager::getShaderIndex(ITF_shader* _shader)
    {
        auto itFound = std::find(mp_shaderList.begin(), mp_shaderList.end(), _shader);
        if (mp_shaderList.end() != itFound)
        {
            return static_cast<i32>(itFound - mp_shaderList.begin());
        }
        else
        {
            return -1; // Not found
        }
    }
    
    ITF_shader* GFXAdapter_shaderManager::getShaderByName(const String& _name)
    {
        auto it = std::find_if(mp_shaderList.begin(), mp_shaderList.end(),
        [&](ITF_shader* shader)
        {
            return shader->m_name == _name;
        });
        return it != mp_shaderList.end() ? *it : nullptr;
    }

    void GFXAdapter_shaderManager::destroyAll()
    {
        for (u32 i = 0; i < mp_shaderList.size(); i++)
		{
            delete mp_shaderList[i];
		}

		mp_shaderList.clear();
    }

} // namespace ITF

#endif // ITF_WII
