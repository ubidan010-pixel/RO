#ifndef _ITF_STATSMANAGER_H_
#define _ITF_STATSMANAGER_H_

#ifdef SUPPORT_STATS_MANAGER

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_


namespace ITF
{
    class Actor;

    struct eventRegister
    {
        eventRegister()
        {
            count   = 0;
            call    = 0;
        };

        u32 count;
        u32 call;
    };

    struct StatsBlock
    {
        u32 m_pickable;
        u32 m_actor;
        u32 m_frize;
        u32 m_actorUpdated;
        u32 m_actorDraw;
        u32 m_actorDraw2D;
        u32 m_friezeUpdate;
        u32 m_friezeDraw;
        u32 m_animLightComponentUpdate;
        u32 m_animLightComponentDraw;
        u32 m_particleUpdate;
        u32 m_particleDraw;
        u32 m_particleComplexNew;
        u32 m_particleComplexActive;
        u32 m_updatePatches;
        u32 m_onActorEvent;
        u32 m_cellEnable;
        u32 m_removedFromCell;
        u32 m_drawMesh_P;
        u32 m_drawMesh_PT;
        u32 m_drawMesh_PC;
        u32 m_drawMesh_PCT;
        u32 m_drawMesh_PCBT;
        u32 m_drawMesh_PNC3T;
        u32 m_drawMesh_unknown;
        u32 m_dynRingVBUsage;
        u32 m_drawVtxCnt;
        u32 m_drawPatchVtxCnt;
        u32 m_drawBezierPatchVtxCnt;
        u32 m_drawPNC3TVtxCnt;
        u32 m_animBonesUpdate;
        u32 m_materialDefault;
        u32 m_materialFrizeAnim;
        u32 m_materialRefraction;
        u32 m_materialGlow;
        u32 m_materialAlphaFade;
        u32 m_renderTarget;

        ITF_MAP<String,eventRegister>     m_countEvent;

        ITF_VECTOR<ObjectRef>   m_ActorsRef;

        void clear();
    };

    //------------------------------------------------------------------------------------------------------------------------------------

	class StatsManager : public TemplateSingleton<StatsManager> 
	{
	public:
		StatsManager() {init();};
       
		~StatsManager() {};

        void init();

		void clear();

		void actorUpdatedInc(Actor* _this);

        ITF_INLINE void pickableInc(u32 _inc)                       {m_statsBlock[m_currentBuffer].m_pickable+=_inc;}
        ITF_INLINE void actorInc(u32 _inc)                          {m_statsBlock[m_currentBuffer].m_actor+=_inc;}
        ITF_INLINE void frizeInc(u32 _inc)                          {m_statsBlock[m_currentBuffer].m_frize+=_inc;}
        ITF_INLINE void actorDrawInc()		                        {m_statsBlock[m_currentBuffer].m_actorDraw++;}
        ITF_INLINE void actorDraw2DInc()                            {m_statsBlock[m_currentBuffer].m_actorDraw2D++;}
        ITF_INLINE void friezeUpdateInc()		                    {m_statsBlock[m_currentBuffer].m_friezeUpdate++;}
        ITF_INLINE void friezeDrawInc()                             {m_statsBlock[m_currentBuffer].m_friezeDraw++;}
        ITF_INLINE void animLightComponentUpdateInc()               {m_statsBlock[m_currentBuffer].m_animLightComponentUpdate++;}
        ITF_INLINE void animLightComponentDrawInc()                 {m_statsBlock[m_currentBuffer].m_animLightComponentDraw++;}
        ITF_INLINE void particleUpdateInc()                         {m_statsBlock[m_currentBuffer].m_particleUpdate++;}
        ITF_INLINE void particleDrawInc()                           {m_statsBlock[m_currentBuffer].m_particleDraw++;}
        ITF_INLINE void particleComplexNewInc()                     {m_statsBlock[m_currentBuffer].m_particleComplexNew++;}
        ITF_INLINE void particleComplexActiveInc()                  {m_statsBlock[m_currentBuffer].m_particleComplexActive++;}
        ITF_INLINE void patchesUpdateInc()                          {m_statsBlock[m_currentBuffer].m_updatePatches++;}
        ITF_INLINE void onActorEventInc()                           {m_statsBlock[m_currentBuffer].m_onActorEvent++;}
        ITF_INLINE void onCellEnableInc()                           {m_statsBlock[m_currentBuffer].m_cellEnable++;}
        ITF_INLINE void onRemoveFromCellInc()                       {m_statsBlock[m_currentBuffer].m_removedFromCell++;}
        ITF_INLINE void drawMeshPInc()                              {m_statsBlock[m_currentBuffer].m_drawMesh_P++;}
        ITF_INLINE void drawMeshPTInc()                             {m_statsBlock[m_currentBuffer].m_drawMesh_PT++;}
        ITF_INLINE void drawMeshPCInc()                             {m_statsBlock[m_currentBuffer].m_drawMesh_PC++;}
        ITF_INLINE void drawMeshPCTInc()                            {m_statsBlock[m_currentBuffer].m_drawMesh_PCT++;}
        ITF_INLINE void drawMeshPCBTInc()                           {m_statsBlock[m_currentBuffer].m_drawMesh_PCBT++;}
        ITF_INLINE void drawMeshPNC3TInc()                          {m_statsBlock[m_currentBuffer].m_drawMesh_PNC3T++;}
        ITF_INLINE void drawMeshUnknownInc()                        {m_statsBlock[m_currentBuffer].m_drawMesh_unknown++;}
        ITF_INLINE void dynRingVBUsageSet(u32 _v)                   {m_statsBlock[m_currentBuffer].m_dynRingVBUsage=_v;}
        ITF_INLINE void drawVtxCntInc(u32 _v)                       {m_statsBlock[m_currentBuffer].m_drawVtxCnt+=_v;}
        ITF_INLINE void drawPatchVtxCntInc(u32 _v)                  {m_statsBlock[m_currentBuffer].m_drawPatchVtxCnt+=_v;}
        ITF_INLINE void drawBezierPatchVtxCntInc(u32 _v)            {m_statsBlock[m_currentBuffer].m_drawBezierPatchVtxCnt+=_v;}
        ITF_INLINE void drawPNC3TVtxCntInc(u32 _v)                  {m_statsBlock[m_currentBuffer].m_drawPNC3TVtxCnt+=_v;}        
        ITF_INLINE void animBonesUpdateInc()                        {m_statsBlock[m_currentBuffer].m_animBonesUpdate++;}
        ITF_INLINE void materialDefaultInc()                        {m_statsBlock[m_currentBuffer].m_materialDefault++;}
        ITF_INLINE void materialFrizeAnimInc()                      {m_statsBlock[m_currentBuffer].m_materialFrizeAnim++;}
        ITF_INLINE void materialRefractionInc()                     {m_statsBlock[m_currentBuffer].m_materialRefraction++;}
        ITF_INLINE void materialGlowInc()                           {m_statsBlock[m_currentBuffer].m_materialGlow++;}
        ITF_INLINE void materialAlphaFadeInc()                      {m_statsBlock[m_currentBuffer].m_materialAlphaFade++;}
        ITF_INLINE void renderTargetInc()                           {m_statsBlock[m_currentBuffer].m_renderTarget++;}

        ITF_INLINE u32 getOnActorEventCount()                       {return m_statsBlock[m_currentBuffer].m_onActorEvent;}
        ITF_INLINE u32 getActor()               const               {return m_statsBlock[m_currentBuffer].m_actor;}
        ITF_INLINE u32 getActorUpdate()         const               {return m_statsBlock[m_currentBuffer].m_actorUpdated;}

        void memObjectInc(const char* _objClass, u32 _v, u32 _classSize);
        void registerEvent(const String& _name,u32 _count);

        void getInfosEvent(ITF_VECTOR<String> &_vStrings);

        void getInfos(ITF_VECTOR<String> &_vStrings);
        void getObjects(class Blob& _blob);

        
	protected:
		
        StatsBlock m_statsBlock[2];
        u32 m_currentBuffer;
        struct ObjMemStats
        {
            u32 m_size;
            u32 m_count;
            u32 m_sizeof;
        };
        ITF_MAP<const char*,ObjMemStats> m_memObjStats;		
	};
}

//------------------------------------------------------------------------------------------------------------------------------------

#define STATS_MANAGER                       StatsManager::getptr()
#define STATS_PICKABLE_INC(v)               StatsManager::getptr()->pickableInc(v);
#define STATS_ACTOR_INC(v)                  StatsManager::getptr()->actorInc(v);
#define STATS_FRIZE_INC(v)                  StatsManager::getptr()->frizeInc(v);
#define STATS_ACTOR_UPDATEINC               StatsManager::getptr()->actorUpdatedInc(this);
#define STATS_ACTOR_DRAWINC                 StatsManager::getptr()->actorDrawInc();
#define STATS_ACTOR_DRAW2DINC               StatsManager::getptr()->actorDraw2DInc();
#define STATS_FRIEZE_UPDATEINC              StatsManager::getptr()->friezeUpdateInc();
#define STATS_FRIEZE_DRAWINC                StatsManager::getptr()->friezeDrawInc();
#define STATS_ANIMLIGHTCOMPONENT_UPDATEINC  StatsManager::getptr()->animLightComponentUpdateInc();
#define STATS_ANIMLIGHTCOMPONENT_DRAWINC    StatsManager::getptr()->animLightComponentDrawInc();

#define STATS_PARTICULE_UPDATEINC           StatsManager::getptr()->particleUpdateInc();
#define STATS_PARTICULE_DRAWINC             StatsManager::getptr()->particleDrawInc();
#define STATS_PARTICULE_COMPLEXNEWINC       StatsManager::getptr()->particleComplexNewInc();
#define STATS_PARTICULE_COMPLEXACTIVEINC    StatsManager::getptr()->particleComplexActiveInc();

#define STATS_PATCHES_UPDATEDINC            StatsManager::getptr()->patchesUpdateInc();

#define STATS_ONACTOREVENTINC               StatsManager::getptr()->onActorEventInc();

#define STATS_CELLENABLEINC                 StatsManager::getptr()->onCellEnableInc();

#define STATS_REMOVEDFROMCELLINC            StatsManager::getptr()->onRemoveFromCellInc();

#define GET_ONACTOREVENTCOUNT               StatsManager::getptr()->getOnActorEventCount();

#define STATS_REGISTEREVENT(__name,__count) StatsManager::getptr()->registerEvent(__name,__count);

#define STATS_ANIM_UPDATEDBONESINC          StatsManager::getptr()->animBonesUpdateInc();

#else

#define STATS_PICKABLE_INC(v)   {}
#define STATS_ACTOR_INC(v)      {}
#define STATS_FRIZE_INC(v)      {}
#define STATS_ACTOR_UPDATEINC   {}
#define STATS_ACTOR_DRAWINC     {}
#define STATS_ACTOR_DRAW2DINC   {}
#define STATS_FRIEZE_UPDATEINC  {}
#define STATS_FRIEZE_DRAWINC    {}

#define STATS_ANIMLIGHTCOMPONENT_UPDATEINC {}
#define STATS_ANIMLIGHTCOMPONENT_DRAWINC   {}

#define STATS_PARTICULE_UPDATEINC          {}
#define STATS_PARTICULE_DRAWINC            {}
#define STATS_PARTICULE_COMPLEXNEWINC      {}
#define STATS_PARTICULE_COMPLEXACTIVEINC   {}

#define STATS_PATCHES_UPDATEDINC           {}

#define STATS_ONACTOREVENTINC              {}

#define STATS_CELLENABLEINC                {}

#define STATS_REMOVEDFROMCELLINC           {}

#define GET_ONACTOREVENTCOUNT              {}

#define STATS_REGISTEREVENT(__name,__count) {}

#define STATS_ANIM_UPDATEDBONESINC          {}
    
#endif //SUPPORT_STATS_MANAGER

//------------------------------------------------------------------------------------------------------------------------------------

#endif //_ITF_STATSMANAGER_H_