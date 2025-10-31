#include "precompiled_engine.h"

#ifdef SUPPORT_STATS_MANAGER

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_STATSMANAGER_H_
#include "engine/stats/statsManager.h"
#endif //_ITF_STATSMANAGER_H_

namespace ITF
{
//------------------------------------------------------------------------------------------------------------------------------------

	void StatsBlock::clear()
	{
        m_pickable                  = 0;
        m_actor                     = 0;
        m_frize                     = 0;
		m_actorUpdated              = 0;
        m_actorDraw                 = 0;
        m_actorDraw2D               = 0;
        m_friezeUpdate              = 0;
        m_friezeDraw                = 0;

        m_animLightComponentUpdate  = 0;
        m_animLightComponentDraw    = 0;

        m_particleUpdate            = 0;
        m_particleDraw              = 0;
        m_particleComplexNew        = 0;
        m_particleComplexActive     = 0;
        m_updatePatches             = 0;
        m_onActorEvent              = 0;
        m_cellEnable                = 0;
        m_removedFromCell           = 0;

        m_drawMesh_P                = 0;
        m_drawMesh_PT               = 0;
        m_drawMesh_PC               = 0;
        m_drawMesh_PCT              = 0;
        m_drawMesh_PCBT             = 0;
        m_drawMesh_PNC3T            = 0;
        m_drawMesh_unknown          = 0;
        m_dynRingVBUsage            = 0;
        m_drawVtxCnt                = 0;
        m_drawPatchVtxCnt           = 0;
        m_drawBezierPatchVtxCnt     = 0;
        m_drawPNC3TVtxCnt           = 0;
        m_animBonesUpdate           = 0;
        m_materialDefault           = 0;
        m_materialFrizeAnim         = 0;
        m_materialRefraction        = 0;
        m_materialGlow              = 0;
        m_materialAlphaFade         = 0;
        m_renderTarget              = 0;

        m_countEvent.clear();

        m_ActorsRef.clear();
    }

//------------------------------------------------------------------------------------------------------------------------------------

    void StatsManager::init()
    {
        
        m_currentBuffer = 0;
        m_statsBlock[0].clear();
        m_statsBlock[1].clear();
    }
//------------------------------------------------------------------------------------------------------------------------------------

    void StatsManager::clear()
    {
        m_currentBuffer = !m_currentBuffer;
        m_statsBlock[m_currentBuffer].clear();
    }

//------------------------------------------------------------------------------------------------------------------------------------

    //NOTE:This function is slow ,use only to track event issue
    void StatsManager::registerEvent(const String& _name,u32 _count)
    {
        ITF_MAP<String,eventRegister>&      mapEvent = m_statsBlock[m_currentBuffer]. m_countEvent;
        ITF_MAP<String,eventRegister>::iterator iter = mapEvent.find(_name);

        if (iter!=mapEvent.end())
        {
            eventRegister& eventR = (*iter).second;
            eventR.count +=_count;
            eventR.call++;
        }
        else
        {
             eventRegister eventR;
             eventR.count = _count;
             eventR.call++;
             mapEvent[_name] = eventR;
        }

    }

//------------------------------------------------------------------------------------------------------------------------------------

#define PUSH_INFO(_name,_value) {info.setTextFormat("%s %d",_name,_value);_vStrings.push_back(info);}

    void StatsManager::getInfos(ITF_VECTOR<String> &_vStrings)
    {
        String info;

        u32 currentBuffer = !m_currentBuffer;

        const StatsBlock& statsBlock = m_statsBlock[currentBuffer];

        PUSH_INFO("pickable",statsBlock.m_pickable);
        PUSH_INFO("actor",statsBlock.m_actor);
        PUSH_INFO("frize",statsBlock.m_frize);
        PUSH_INFO("actorUpdated",statsBlock.m_actorUpdated);
        PUSH_INFO("actorDraw",statsBlock.m_actorDraw);
        PUSH_INFO("actorDraw2D",statsBlock.m_actorDraw2D);
        PUSH_INFO("friezeUpdate",statsBlock.m_friezeUpdate);
        PUSH_INFO("friezeDraw",statsBlock.m_friezeDraw);
        PUSH_INFO("animLightComponentUpdate",statsBlock.m_animLightComponentUpdate);
        PUSH_INFO("animLightComponentDraw",statsBlock.m_animLightComponentDraw);
        PUSH_INFO("particleUpdate",statsBlock.m_particleUpdate);
        PUSH_INFO("particleDraw",statsBlock.m_particleDraw);
        PUSH_INFO("particleComplexNew",statsBlock.m_particleComplexNew);
        PUSH_INFO("particleComplexActive",statsBlock.m_particleComplexActive);
        PUSH_INFO("updatePatches",statsBlock.m_updatePatches);
        PUSH_INFO("onActorEvent",statsBlock.m_onActorEvent);
        PUSH_INFO("cellEnable",statsBlock.m_cellEnable);
        PUSH_INFO("objRemovedFromCell",statsBlock.m_removedFromCell);
        info.setTextFormat("drawMesh (P)%- 3d (PT)%- 3d (PC)%- 3d",statsBlock.m_drawMesh_P, statsBlock.m_drawMesh_PT, statsBlock.m_drawMesh_PC);_vStrings.push_back(info);
        info.setTextFormat("drawMesh (PCT)%- 3d (PCBT)%- 3d (PNC3T)%- 3d", statsBlock.m_drawMesh_PCT, statsBlock.m_drawMesh_PCBT, statsBlock.m_drawMesh_PNC3T);_vStrings.push_back(info);
        /*PUSH_INFO("drawMesh_P",statsBlock.m_drawMesh_P);
        PUSH_INFO("drawMesh_PT",statsBlock.m_drawMesh_PT);
        PUSH_INFO("drawMesh_PC",statsBlock.m_drawMesh_PC);
        PUSH_INFO("drawMesh_PCT",statsBlock.m_drawMesh_PCT);
        PUSH_INFO("drawMesh_PCBT",statsBlock.m_drawMesh_PCBT);
        PUSH_INFO("drawMesh_PNC3T",statsBlock.m_drawMesh_PNC3T);*/
    #if defined(VBMANAGER_USE_DYNAMICRING_VB)
        if(GFX_ADAPTER->getVertexBufferManager().getDynamicRingVBSize())
        {
            info.setTextFormat("dynRingVBUsage %d (%.1f%%)",statsBlock.m_dynRingVBUsage,(100.f*(f32)statsBlock.m_dynRingVBUsage)/(f32)GFX_ADAPTER->getVertexBufferManager().getDynamicRingVBSize());
            _vStrings.push_back(info);
        }
    #endif
        PUSH_INFO("drawVtxCnt",statsBlock.m_drawVtxCnt);
        PUSH_INFO("drawPatchVtxCnt",statsBlock.m_drawPatchVtxCnt);
        PUSH_INFO("drawBezierPatchVtxCnt",statsBlock.m_drawBezierPatchVtxCnt);
        PUSH_INFO("drawPNC3TVtxCnt",statsBlock.m_drawPNC3TVtxCnt);
        PUSH_INFO("animBonesUpdate",statsBlock.m_animBonesUpdate);

        info.setTextFormat("material (Def)%- 3d (FAnim)%- 3d (Refra)%- 3d (Glow)%- 3d (AlpFade)%- 3d", statsBlock.m_materialDefault, statsBlock.m_materialFrizeAnim, statsBlock.m_materialRefraction, statsBlock.m_materialGlow, statsBlock.m_materialAlphaFade);_vStrings.push_back(info);
        PUSH_INFO("renderTargetUsage",statsBlock.m_renderTarget);

#if defined( DEVELOPER_ATN )
        {
            for (ITF_MAP<const char*,ObjMemStats>::iterator iter = m_memObjStats.begin();iter!=m_memObjStats.end();iter++)
            {
                if((*iter).second.m_size)
                {
                    info.setTextFormat("%s\t%d\t%d\t%d",(*iter).first,(*iter).second.m_size, (*iter).second.m_count, (*iter).second.m_sizeof);
                    _vStrings.push_back(info);
                }
            }
        }
#endif
    }


//------------------------------------------------------------------------------------------------------------------------------------

    void StatsManager::getInfosEvent(ITF_VECTOR<String> &_vStrings)
    {
        String info;

        u32 count = 0;
#define PUSH_INFOEVENT(_name,_call,_count) info.setTextFormat("%ls call:%d count:%d",_name,_call,_count);_vStrings.push_back(info);

        ITF_MAP<String,eventRegister>& mapEvent = m_statsBlock[m_currentBuffer]. m_countEvent;
        _vStrings.clear();
        
        LOG("----------------------------------------------------------");
        for (ITF_MAP<String,eventRegister>::iterator iter = mapEvent.begin();iter!=mapEvent.end();iter++)
        {
            eventRegister& eventR = (*iter).second;
            PUSH_INFOEVENT((*iter).first.cStr(),eventR.call,eventR.count);

            count+=eventR.call*eventR.count;

            LOG(">>%ls",info.cStr());
        }

        LOG("----------------------------------------------------------");
        info.setTextFormat("Global Event isClass: %d",count);
        _vStrings.push_back(info);
    }

//------------------------------------------------------------------------------------------------------------------------------------

    void StatsManager::actorUpdatedInc(Actor* _this)
    {
        StatsBlock& statsBlock = m_statsBlock[m_currentBuffer];
        statsBlock.m_ActorsRef.push_back(_this->getRef());
        statsBlock.m_actorUpdated++;
    }

//------------------------------------------------------------------------------------------------------------------------------------

    void StatsManager::getObjects(Blob& _blob)
    {
        //send ref ID + aabb
        
        u32 currentBuffer = !m_currentBuffer;

        const StatsBlock& statsBlock = m_statsBlock[currentBuffer];

        _blob.pushUInt32(statsBlock.m_ActorsRef.size());

        for (ITF_VECTOR<ObjectRef>::const_iterator iter = statsBlock.m_ActorsRef.begin();iter != statsBlock.m_ActorsRef.end();++iter )
        {
            const Actor* pActor = static_cast<const Actor*>(GETOBJECT(*iter));
            if (pActor)
            {
                _blob.pushUInt32((*iter).getValue());

                const AABB& aabb = pActor->getAABB();

                _blob.pushFloat32(aabb.getMin().m_x);
                _blob.pushFloat32(aabb.getMin().m_y);
                _blob.pushFloat32(aabb.getMax().m_x);
                _blob.pushFloat32(aabb.getMax().m_y);
            }
               
        }

    }

    void StatsManager::memObjectInc(const char* _objClass, u32 _v, u32 _classSize)
    {
        ITF_MAP<const char*,ObjMemStats>::iterator iter = m_memObjStats.find(_objClass);
        if(iter == m_memObjStats.end())
        {
            ObjMemStats m = {_v,1,_classSize};
            m_memObjStats[_objClass] = m;
        }
        else
        {
            (*iter).second.m_size += _v;
            (*iter).second.m_count++;
        }
    }
}



#endif //SUPPORT_STATS_MANAGER
