#ifndef __WWISE_EDITORPLUGIN_H__
#define __WWISE_EDITORPLUGIN_H__

#if defined ITF_SUPPORT_WWISE && !defined ITF_FINAL && defined ITF_SUPPORT_EDITOR
#include "tools/plugins/ActorEditor/ActorEditor.h"

namespace	ITF
{
	class	SoundDescriptor_Template;
	struct AudioPlayRequest;

	class WWISE_EditorPlugin :
		public Interface_ActorEditorListener
	{


	public:
			static WWISE_EditorPlugin				*s_getSingleton()	{ return m_singleton; }
			static Interface_ActorEditorListener	*create();
			virtual void							destroy();

			WWISE_EditorPlugin();
			virtual									~WWISE_EditorPlugin()															{ }
			virtual void							onSelected(PickingShape *_pickingShape)											{ }
			virtual void							onObjectDuplicated(Actor *_pActor, Actor *_pActorSrc, ActorComponent *_pComp)	{ }
			virtual void							onObjectChanged(Actor *_pActor, ActorComponent *_pComp)							{ }
			virtual void							onObjectDeleted(BaseObject *_object)											{ }
			virtual void							fillContextMenu(EditorContextMenu &_menu, Actor *_pActor, ActorComponent *_pComp);
			virtual void							onContextMenuItemSelected(ContextMenuItem *_item, Actor *_pActor, ActorComponent *_pComp);
			virtual void							fillAltContextMenu(EditorContextMenu &_menu);
			virtual void							onAltContextMenuItemSelected(ContextMenuItem *_item);

			virtual void							onUpdate()	{ }
			virtual void							onDraw();
			virtual void							onDrawShapeOwned(PickingShape *_shape)																									{ }
			virtual void							onUpdateSelectedActor(Actor *_pActor, ActorComponent *_pComp)																			{ }
			virtual void							onDrawSelectedActor(Actor *actor, ActorComponent *_component)																			{ }
			virtual void							addComponentSpecificIcons(Actor *_pActor, ActorComponent *_pComp, SafeArray<Editor_IconStock::EditorIconID> &_icons, f32 &_IconSize)	{ }
			virtual void							addPickingShapeSpecificIcons(PickingShape *_shape, SafeArray<Editor_IconStock::EditorIconID> &_icons, f32 &_IconSize, Color &_iconColor){ }
			virtual void							fillMainMenu(EditorContextMenu &_menu);
			//virtual bbool                           onMainMenuItemSelected(ContextMenuItem *_item);
			virtual void							deleteSubShapesSameCreator(Actor *_actor, ITF_VECTOR<PickingShape *> &_shapeList)									{ }
			virtual void							fillEditorActiveList(ObjectRefList &_pickablesObjects, PickingShapeVector &_orderedShapes, Scene *_pFilterScene)	{ }
			virtual void							onSetShapePosition(PickingShape *_pPickingShape, const Vec2d &_screenspacepos, Vec3d &_deltaWorldApplied)			{ }
			//virtual void							onOffsetShapePosition(PickingShape *_pPickingShape, const Vec3d &_deltaPos)											{ }
			virtual void							onShapePosChanged(Actor *_pActor, ActorComponent *_pComp, PickingShape *_pShape, const Vec3d &_deltaPos)			{ }
			virtual bbool							needsInitPosPickingShape(Actor *_pActor, ActorComponent *_pComp)													{ return bfalse; }
			virtual void							addComponentSpecificPickingShape(Actor *_pActor, ActorComponent *_pComp)											{ }
			virtual void							updateComponentSpecificPickingShape(Actor *_pActor, PickingShape *_pShape, PickingShapeVector &_activeShapes)		{ }
			static void								s_drawPlayingInstanceCallback(void *, const AudioPlayRequest &);


	private:
			static WWISE_EditorPlugin	*m_singleton;
			
			u32							m_showMask;
			

			struct Instance
			{
				Vec3d		pos;
				const char	*gaoName;
				const char	*soundName;
				f32			radius;
			};

			typedef ITF_MAP<void *, ITF_VECTOR<Instance> >	InstancesMap;
			InstancesMap									m_instancesMap;

			
			void											drawSoundInstances();
			void											draw(const AudioPlayRequest &);
			void											drawEffects();
	};
}	// namespace ITF
#endif
#endif // __WWISE_EDITORPLUGIN_H__
