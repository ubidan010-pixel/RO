#include "precompiled_engine.h"

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_AFTERFX_H_
#include "AfterFx.h"
#endif //_ITF_AFTERFX_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

namespace ITF
{
    BEGIN_SERIALIZATION(AFTERFX)
    SERIALIZE_ENUM_BEGIN("Type",m_type);
        SERIALIZE_ENUM_VAR(AFX_None);
        SERIALIZE_ENUM_VAR(AFX_Blur);
        SERIALIZE_ENUM_VAR(AFX_Glow);
        SERIALIZE_ENUM_VAR(AFX_Remanence);
        SERIALIZE_ENUM_VAR(AFX_DOF);
        SERIALIZE_ENUM_VAR(AFX_RayCenter);
        SERIALIZE_ENUM_VAR(AFX_ColorSetting);
        SERIALIZE_ENUM_VAR(AFX_ColorRemap);
        SERIALIZE_ENUM_VAR(AFX_ColorLevels);
        SERIALIZE_ENUM_VAR(AFX_Fade);
        SERIALIZE_ENUM_VAR(AFX_Bright);
        SERIALIZE_ENUM_VAR(AFX_ToneMap);
        SERIALIZE_ENUM_VAR(AFX_AddSceneAndMul);
        SERIALIZE_ENUM_VAR(AFX_objectsGlow);
        SERIALIZE_ENUM_VAR(AFX_simpleBlend);
    SERIALIZE_ENUM_END();
    SERIALIZE_CONTAINER("paramF",m_Paramf);
    SERIALIZE_CONTAINER("paramI",m_Parami);
    SERIALIZE_CONTAINER("paramV",m_Paramv);
    SERIALIZE_CONTAINER("paramC",m_Paramc);
    SERIALIZE_MEMBER("lifeTime",m_lifeTime);
    SERIALIZE_MEMBER("colorTarget",m_colorTarget);
    END_SERIALIZATION()

///----------------------------------------------------------------------------//
/// Manager.
///----------------------------------------------------------------------------//
 
    AFTERFXManager::~AFTERFXManager()
    { 
        deleteAll();
    }
        
    void AFTERFXManager::addFxPrimitive()
    {
        if (!m_AFXGroupList.size()) return;

        for (u32 i = 0; i < m_AFXGroupList.size() ; i++ )
        {
            AFX_Group* group = m_AFXGroupList[i];

            if ( group->isRenderInTarget() )
            {
                f32 zs = group->getStartRank() + group->getDepth() * 5.f;
                GFX_ADAPTER->addPrimitiveInZList((void*)group, GFX_ITF_STARTRENDERTEX, NULL, zs,0,ObjectRef::InvalidRef);
            }

            f32 zr = group->getRenderRank() + group->getDepth() * 5.f;
            GFX_ADAPTER->addPrimitiveInZList((void*)group, GFX_ITF_AFTERFXGROUP, NULL, zr,0,ObjectRef::InvalidRef);
        }
    }

    void AFTERFXManager::addFxGroup( AFX_Group* _afxg )
    {
        if (!_afxg ) return;
        m_AFXGroupList.push_back(_afxg);
    }

    void AFTERFXManager::deleteAll( )
    {
        for(u32 i = 0; i < m_AFXGroupList.size(); i++)
            delete m_AFXGroupList[i];
            
        m_AFXGroupList.clear();
    }
    
    void AFTERFXManager::update()
    {
    
    }

///----------------------------------------------------------------------------//
/// Group AfterFx.
///----------------------------------------------------------------------------//

    AFX_Group::~AFX_Group()
    {
        destroy();
    }

    void AFX_Group::copy(const AFX_Group& _from)
    {
        this->destroy();
        
        m_renderintarget = _from.m_renderintarget;
        m_startRank = _from.m_startRank;
        m_renderRank = _from.m_renderRank;
        m_finalblend = _from.m_finalblend;
        
        for (u32 i = 0; i < _from.m_AFXList.size(); i++)
        {
            addAFXtoGroup(_from.m_AFXList[i]);
        }   
    }

    void AFX_Group::addAFXtoGroup( AFTERFX* _afx)
    {
        m_AFXList.push_back(_afx);
    }

    void AFX_Group::destroy()
    {
        for(u32 i = 0; i < m_AFXList.size(); i++)
            delete m_AFXList[i];
            
        m_AFXList.clear();
    }

///----------------------------------------------------------------------------//
/// AfterFx.
///----------------------------------------------------------------------------//

    AFTERFX_Type AFTERFX::getTypeByName(String& _name)
    {
        String name = _name;
        name.toLower();

        if (name == "blur") return AFX_Blur;
        else if (name == "glow") return AFX_Glow;
        else if (name == "remanence") return AFX_Remanence;
        else if (name == "dof") return AFX_DOF;
        else if (name == "godray") return AFX_RayCenter;
        else if (name == "colorsetting") return AFX_ColorSetting;
        else if (name == "colorremap") return AFX_ColorRemap;
        else if (name == "colorlevels") return AFX_ColorLevels;
        else if (name == "fade") return AFX_Fade;
        else if (name == "brightpass") return AFX_Bright;
        else if (name == "tonemap") return AFX_ToneMap;
        else if (name == "addsceneandmul") return AFX_AddSceneAndMul;
        else if (name == "simpleblend") return AFX_simpleBlend;
        return AFX_None;
    }

    void AFTERFX::destroy()
    {
        /*for (u32 i = 0; i < 4; i++)
        {
            if (m_customTextureID[i].isValidResourceId())
            {
                removeResource(m_customTextureID[i]);
                m_customTextureID[i].invalidateResourceId();
            }
        }*/
    }

    ResourceGroup* AFTERFX::getResourceGroup() 
    {
        if (!m_resourceGroup.isValidResourceId())
            m_resourceGroup = RESOURCE_MANAGER->newResourceIDFromFile(Resource::ResourceType_ResourceGroup, Path::EmptyPath);

        return (ResourceGroup*)(m_resourceGroup.getResource());
    }   

    /*ResourceID AFTERFX::addResource( Resource::ResourceType _type, const Path &_path )
    {
        return getResourceGroup()->addResource(_type,_path);
    }
*/
    void AFTERFX::removeResource(const ResourceID &_resource)
    {
        ResourceGroup* grp = getResourceGroup();

        if (grp)
            grp->removeResource(_resource);
    }

    void AFTERFX::LoadTexture(const Path& path, u32 _n)
    {
        /*if (_n >= 4) return;
        
        if (m_customTextureID[_n].isValidResourceId())
        {
            removeResource(m_customTextureID[_n]);
            m_customTextureID[_n].invalidateResourceId();
        }

        m_customTextureID[_n] = addResource(Resource::ResourceType_Texture, path);*/
    }

    void AFTERFX::loadResources()
    {
        //RESOURCE_MANAGER->addUserToResourcePhysicalData(m_resourceGroup);
    }

    Texture* AFTERFX::getTexture(u32 _n)
    {
        if (m_customTextureID[_n].isValidResourceId() && _n < 4)
        {
            Texture* texture = (Texture*)m_customTextureID[_n].getResource();

            if ( texture && texture->isPhysicallyLoaded())
                return texture;
        }
    
        return NULL;
    }

    void AFTERFX::setTexture(u32 _n, ResourceID _texture)
    {
        ITF_ASSERT(_n < 4);
        m_customTextureID[_n] = _texture;
    }

} // namespace ITF


