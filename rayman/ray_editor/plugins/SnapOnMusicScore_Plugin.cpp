#include "precompiled_ray_editor.h"

// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif //_ENGINECOMMON_H_

#ifndef _ITF_SNAPONMUSICSCORE_PLUGIN_H_
#include "rayman/ray_editor/plugins/SnapOnMusicScore_Plugin.h"
#endif //_ITF_SNAPONMUSICSCORE_PLUGIN_H_

#ifndef _ITF_RAY_MUSICSCORESNAPCOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_MusicScoreSnapComponent.h"
#endif //_ITF_RAY_MUSICSCORESNAPCOMPONENT_H_

#ifndef _ITF_RAY_MUSICSCOREAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_MusicScoreAIComponent.h"
#endif //_ITF_RAY_MUSICSCOREAICOMPONENT_H_

#ifndef _ITF_EDITOR_H_
#include "editor/Editor.h"
#endif //_ITF_EDITOR_H_


namespace ITF
{
    SnapOnMusicScore_Plugin* SnapOnMusicScore_Plugin::s_instance = NULL;

    Interface_ActorEditorListener* SnapOnMusicScore_Plugin::create()
    {
        ITF_ASSERT(!s_instance);
        s_instance = newAlloc(mId_Editor, SnapOnMusicScore_Plugin());
        ActorEditor::getInstance()->registerListener(s_instance, Ray_MusicScoreSnapComponent::GetClassCRCStatic());
        return s_instance;
    }

    // ********************************************************************

    void SnapOnMusicScore_Plugin::destroy()
    {
        ITF_ASSERT(s_instance);
    }

    // ********************************************************************

    SnapOnMusicScore_Plugin::SnapOnMusicScore_Plugin()
    {
        //none
    }

    // ********************************************************************

    SnapOnMusicScore_Plugin::~SnapOnMusicScore_Plugin()
    {
        // none
    }

    // ********************************************************************

    void SnapOnMusicScore_Plugin::onObjectChanged( Actor* _pActor, ActorComponent* _pComp )
    {
        Ray_MusicScoreSnapComponent* snapCompo = static_cast<Ray_MusicScoreSnapComponent*>(_pComp);
        if (!snapCompo)
        {
            return;
        }

        // newly bound? => register and snap
        ActorRef parentRef = snapCompo->getParent();
        ActorBind* actorBind = _pActor->getParentBind();
        if (actorBind)
        {
            ActorRef actorBindParentRef = actorBind->m_runtimeParent;
            if (actorBindParentRef.isValid() &&
                parentRef != actorBindParentRef)
            {
                Actor* actParent = actorBindParentRef.getActor();
                if (actParent)
                {
                    // disable actor-bind flip
                    actorBind->m_useParentFlip = bfalse;
                
                    // register to music score
                    snapCompo->registerMusicScore(actParent, bfalse);
                    
                    // snap
                    snap(_pActor, snapCompo);      
                }
            }
        }
        // newly unbound? => unregister
        else
        {
            if (parentRef.isValid())
            {
                Actor* actParent = parentRef.getActor();
                if (actParent)
                {
                    // unregister from music score
                    snapCompo->unregisterMusicScore(actParent, bfalse);
                }
            }
        }
    }

    // ********************************************************************

    void SnapOnMusicScore_Plugin::onShapePosChanged( Actor* _pActor, ActorComponent* _pComp, PickingShape* _pShape, const Vec3d& /*_deltaPos*/ )
    {
        Ray_MusicScoreSnapComponent* snapCompo = static_cast<Ray_MusicScoreSnapComponent*>(_pComp);
        snap(_pActor, snapCompo);      
    }

    // ********************************************************************

    void SnapOnMusicScore_Plugin::snap( Actor* _pActor, Ray_MusicScoreSnapComponent* _pComp, bbool _fromMusicScore /*= bfalse*/ )
    {
        if (_pActor)
        {
            if (_pComp)
            {
                ActorRef parentRef = _pComp->getParent();
                if (parentRef.isValid())
                {
                    Actor* actParent = parentRef.getActor();

                    if (actParent)
                    {
                        Ray_MusicScoreAIComponent* musicScoreAIComponent = actParent->GetComponent<Ray_MusicScoreAIComponent>();

                        if (musicScoreAIComponent)
                        { 
                            Vec3d refPos = _pActor->getPos();
                            
                            Vec3d snapPos;
                            f32 distOnCurve;
                            u32 selectedNote; 
                            f32 intervalNote;
                            bbool badSnap;
                            bbool reverse = _pComp->getReverse();

                            if (_fromMusicScore)
                            {
                                // refresh position
                                distOnCurve = _pComp->getDistOnCurve();
                                selectedNote = _pComp->getNote();
                                musicScoreAIComponent->getPosAtDist(snapPos, distOnCurve, selectedNote, intervalNote, badSnap, reverse);
                            }
                            else
                            {
                                // get info from MusicScore
                                musicScoreAIComponent->getClosestPos(_pActor->getPos(), snapPos, distOnCurve, selectedNote, intervalNote, badSnap, reverse);
                            }

                            // set info on object snap
                            _pComp->setInfoSnap(distOnCurve, selectedNote, intervalNote, badSnap, reverse);
                            
                            const bbool allowedToEditInitialValues = EDITOR->isAllowStorePickableInitData(_pActor);
                            if(allowedToEditInitialValues)
                            {
                                _pActor->setWorldInitialPos(snapPos);
                                _pActor->setBoundWorldInitialPos(snapPos);
                            }
                            _pActor->setPos(snapPos);
                            
                            // save
                            _pActor->replaceReinitDataByCurrent();

                            // force open (to ease editing)
                            musicScoreAIComponent->forceOpen();
                        }
                    }
                }
            }
        }
    }

    // ********************************************************************


} // namespace ITF
