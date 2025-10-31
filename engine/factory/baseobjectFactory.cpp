#include "precompiled_engine.h"

#ifndef _ITF_BASEOBJECTFACTORY_H_
#include "engine/factory/baseObjectFactory.h"
#endif //_ITF_BASEOBJECTFACTORY_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_SOUND_H_
#include "engine/sound/Sound.h"
#endif //_ITF_SOUND_H_

#ifndef _ITF_ANIMATIONRES_H_
#include "engine/animation/AnimationRes.h"
#endif //_ITF_ANIMATIONRES_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

namespace ITF
{
    BaseObjectFactory::BaseObjectFactory()
    {

        m_objectFactory.RegisterObject<Scene>(ITF_GET_STRINGID_CRC(Scene,209039730));
        m_objectFactory.RegisterObject<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
        m_objectFactory.RegisterObject<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817));
        m_objectFactory.RegisterObject<Frise>(ITF_GET_STRINGID_CRC(Frise,2579114664));
        m_objectFactory.RegisterObject<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));
        m_objectFactory.RegisterObject<Sound>(ITF_GET_STRINGID_CRC(Sound,2608772127));
        m_objectFactory.RegisterObject<Texture>(ITF_GET_STRINGID_CRC(Texture,3846959819));
        //m_objectFactory.RegisterObject<Animation>(ITF_GET_STRINGID_CRC(Animation,3251806168));
    }


} // namespace ITF
