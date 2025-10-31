#include "precompiled_engine.h"

#ifndef _ITF_PICKABLE_H_
#include "engine/picking/Pickable.h"
#endif // _ITF_PICKABLE_H_

#ifndef _ITF_OBJECTGROUP_H_
#include "engine/scene/ObjectGroup.h"
#endif //_ITF_OBJECTGROUP_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_LIGHTCOMPONENT_H_
#include "gameplay/components/misc/LightComponent.h"
#endif //_ITF_LIGHTCOMPONENT_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

namespace ITF
{

#ifdef ITF_SUPPORT_EDITOR
    // see implementation in Editor.cpp
#else
    void ObjectGroup::update( f32 _elapsed )
    {
        u32 size = m_elements.size();
        for (u32 i = 0; i < size; i++)
            m_elements[i]->update(_elapsed);
        m_elements.clear();
    }
    
    void ObjectGroup::draw(SafeArray<class LightComponent*>& _activeLights)
    {
        u32 numberLights = _activeLights.size();
        u32 size = m_elements.size();
        for (u32 i = 0; i < size; i++)
        {
            Pickable* pObj = m_elements[i];
            if (numberLights && pObj->getObjectType() == BaseObject::eActor)
            {
                const Actor* pAct = static_cast<const Actor*>(pObj);
                AnimLightComponent *pAnimLight = pAct->GetComponent<AnimLightComponent>();
                if (pAnimLight)
                {
                    pAnimLight->clearLights();
                    for (u32 il = 0 ; il < numberLights ; il++)
                    {
                        LightComponent *light = _activeLights[il];
                        light->processLight(m_elements[i]);
                    }
                }
            }
            pObj->draw();
        }

         m_elements.clear();
    }
#endif
/*
    void ObjectGroup::removeElement( Pickable* _elem )
    {
        i32 index = m_elements.find(_elem);
        if (index >= 0)
            m_elements.eraseKeepOrder(index);
    }
    */

    
    void ObjectGroup::postUpdate()
    {
        // Reverse parsing to respect parent child update order
        for(i32 i = m_subsceneAABBrefresh.size() - 1; i >= 0; --i)
        {
            SubSceneActor* pSSA = m_subsceneAABBrefresh[i];

            if(SubSceneActor* pParentSSA = pSSA->getScene()->getSubSceneActorHolder())
                pParentSSA->growInternalRelativeAABBFromAbsolute(pSSA->getAABB(), pSSA->getDepth());
        }

        m_subsceneAABBrefresh.clear();
    }

    void ObjectGroup::removeElement( BaseObject* _pObj)
    {
        if (i32 found = m_elements.find(static_cast<Pickable*>(_pObj)) >= 0)
            m_elements.eraseKeepOrder(found);
        if (i32 found = m_subsceneAABBrefresh.find(static_cast<SubSceneActor*>(_pObj)) >= 0)
            m_subsceneAABBrefresh.eraseKeepOrder(found);
    }

}

