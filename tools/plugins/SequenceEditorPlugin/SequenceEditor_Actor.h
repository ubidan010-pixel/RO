#ifndef _ITF_SEQUENCEEDITORACTOR_H_
#define _ITF_SEQUENCEEDITORACTOR_H_

#ifndef _ITF_BLOB_H_
#include "core/Blob.h"
#endif //_ITF_BLOB_H_




namespace ITF
{

struct PickingInfo2D;
class SequencePlayerComponent;
class AnimatedComponent;
class Actor;
class SequenceEditorPlugin;
class AnimLightComponent;
class SubAnimSet;
class AnimTrack;
class PlayAnim_evt;
class Pickable;

class SequenceEditorActor
{
public:
    SequenceEditorActor ();
    bbool       onActorPicked(ObjectRef _actorRef);
    bbool       onActorSubEdit(ObjectRef _actorRef);
    void        getSelectionStatus (ObjectRef& _selection, bbool& _selectionIsLocked);
    void        onPickableRenamed(Pickable* _pickable, const String8& _name);
    void        onReceiveSequence(Blob& _blob);
    void        setOwner(SequenceEditorPlugin* _plug)    {m_owner = _plug;}  
    void        sendInfoListAnimationsActor(Blob& _blob, Actor* _actor, const String8 _friendlyName);
    void        sendInfoListSubAnimsActor(Blob& _blob, Actor* actor, const String8 _friendlyName);
    void        sendInfoListAnimations(Blob& _blob, String _animScene);
    void        sendInfoListSoundActor(Blob& _blob, Actor* actor, const String8 _friendlyName);
    void        setPreviewPlayAnim( Blob& _blob );
    void        stopPreviewPlayAnim( Blob& _blob );
    void        update();
    ObjectRef   getSeqPlaySelected() { return m_seqPlayCompSelected; }

private:
    void    onSequencePlayerActorPicked(Actor* _pActor, Blob& _blob);
    void    getInfoListAnimationActor(String _cmd, Actor* _pActor, Blob& _blob);
    void    getListOfMarkerAnimation(Blob& _blob, AnimTrack *_pAnimTrack);
    
    SequenceEditorPlugin* m_owner;
    ObjectRef m_seqPlayCompSelected;
    PlayAnim_evt* m_eventPreviewAnim;
    u32 m_framePreviewAnim;
};

} // namespace ITF


#endif // _ITF_SEQUENCEEDITORACTOR_H_
