#ifndef _ITF_RAY_DJEMBEFRIEZEPLUGIN_H_
#define _ITF_RAY_DJEMBEFRIEZEPLUGIN_H_

#ifndef _ITF_FRIEZEPLUGIN_H_
#include "engine/display/Frieze/FriezePlugin.h"
#endif

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

namespace ITF
{
    class Frise;

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Plugin for djembe friezes
    class Ray_DjembeFriezePlugin : public FriezePlugin
    {
    public:
        Ray_DjembeFriezePlugin(Frise *_frieze)
            : FriezePlugin(_frieze)
            , m_alpha(0)
            , m_alphaSpeed(0)
            , m_bubblePrizeHub()
            {}

        virtual void buildFrieze_static(Frise::MeshStatic_VertexList& _vertexTempList, Frise::MeshStatic_IndexList* _indexTempList);
        virtual void update(f32 _dt);
        virtual void onEvent(Event *_event);

        static FriezePlugin *creationFunction(Frise *_frieze) {return newAlloc(mId_Gameplay,  Ray_DjembeFriezePlugin(_frieze));}

    private:
        template<u32 xCount, u32 yCount>
        void generateTessellatedQuad(Frise::MeshStatic_VertexList& _vertexTempList, Frise::MeshStatic_IndexList &_indexTempList, Vec2d *_pos, Vec2d *uv, u32 *_colors);
        void processContact( PolyLine* _poly, u32 _edge, ActorRef _activator );

        f32 m_alpha;
        f32 m_alphaSpeed;

        ActorRef m_bubblePrizeHub;
    };
}


#endif
