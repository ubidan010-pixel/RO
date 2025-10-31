#include "precompiled_Adapter_WWISE.h"
#ifdef todoisap
#if !defined ITF_FINAL && defined ITF_SUPPORT_EDITOR
#include "adapters/Adapter_WWISE/EditorPlugin/WWISE_EditorPlugin.h"
#include "adapters/AudioSerializedData/Wwise/WwiseSoundComponent.h"
#include "engine/sound/MetronomeManager.h"
#include "editor/editor.h"
#include "adapters/Adapter_WWISE/Helper/WWISE_AkGameObject.h"
#include "AK/SoundEngine/Common/AkQueryParameters.h"
#include "editor/EditorDraw.h"
#include "adapters/Adapter_WWISE/EditorPlugin/WWISE_EditorMenuID.h"
#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"
#include "adapters/Adapter_WWISE/Wrapper/WWISE_Wrapper.h"

namespace	ITF
{
	enum ShowMask
	{
		SHOW_NONE				= 0,
		SHOW_METRONOME			= 1,
		SHOW_ALL_INSTANCES		= 2,
		SHOW_SELECTED_INSTANCES = 8,
		SHOW_EFFECTS			= 0x10,
	};

	WWISE_EditorPlugin *WWISE_EditorPlugin::	m_singleton = NULL;

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//

	Interface_ActorEditorListener *WWISE_EditorPlugin::create()
	{
		AUDIO_ASSERT(!m_singleton, "WWISE_EditorPlugin exist !");
		m_singleton = newAlloc(mId_Editor, WWISE_EditorPlugin());

		ActorEditor::getInstance()->registerListener(m_singleton, SoundComponent::GetClassCRCStatic());
		return m_singleton;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void WWISE_EditorPlugin::destroy()
	{
		AUDIO_ASSERT(m_singleton, "WWISE_EditorPlugin singleton is !=NULL");
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	WWISE_EditorPlugin::WWISE_EditorPlugin() :
		m_showMask(SHOW_NONE)
	{
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void WWISE_EditorPlugin::fillContextMenu(EditorContextMenu &_menu, Actor *_pActor, ActorComponent *_pComp)
	{
		ContextMenuItem *soundMenuItem = _menu.getItem(EditorContextMenu::ActorCtxtMenu_SoundRangeStart);
		if(!soundMenuItem)
		{
			 _menu.addItemFromExternalPlugin(EditorContextMenu::ActorCtxtMenu_SoundRangeStart, "Sound");
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void WWISE_EditorPlugin::onContextMenuItemSelected(ContextMenuItem *_item, Actor *_pActor, ActorComponent *_pComp)
	{
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void WWISE_EditorPlugin::fillAltContextMenu(EditorContextMenu &_menu)
	{
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void WWISE_EditorPlugin::onAltContextMenuItemSelected(ContextMenuItem *_item)
	{
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void WWISE_EditorPlugin::fillMainMenu(EditorContextMenu &_menu)
	{
		ContextMenuItem *SoundMenuItem = _menu.getItem(EditorContextMenu::AltRClickMenu_SoundRangeStart);

		if(!SoundMenuItem)
		{
			SoundMenuItem = _menu.addItemFromExternalPlugin(EditorContextMenu::AltRClickMenu_SoundRangeStart, "Sound");
		}

		if(SoundMenuItem)
		{
			EditorContextMenu	&soundMenu = SoundMenuItem->m_subMenu;

            soundMenu.addCheckableItemFromExternalPlugin(WwiseTopBarMenuID_Cmd_ShowMetronome, "Show Metronome", m_showMask & SHOW_METRONOME?btrue:bfalse);
			soundMenu.addCheckableItemFromExternalPlugin(WwiseTopBarMenuID_Cmd_ShowEffects, "Show Effects", m_showMask & SHOW_EFFECTS?btrue:bfalse);

            
			ContextMenuItem *soundInstanceMenu = soundMenu.addItemFromExternalPlugin(WwiseTopBarMenuID_MenuID_SoundInstances, "Instances");
			soundInstanceMenu->m_subMenu.addCheckableItemFromExternalPlugin(WwiseTopBarMenuID_Cmd_SoundInstances_ShowAll, "Show All", m_showMask & SHOW_ALL_INSTANCES? btrue:bfalse);
			soundInstanceMenu->m_subMenu.addCheckableItemFromExternalPlugin(WwiseTopBarMenuID_Cmd_SoundInstances_ShowOnlySelected, "Show Only Selected", m_showMask & SHOW_SELECTED_INSTANCES? btrue:bfalse);
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	bbool WWISE_EditorPlugin::onMainMenuItemSelected(ContextMenuItem *_item)
	{
		switch(_item->getIdFromExternal())
		{
		case WwiseTopBarMenuID_Cmd_DisplaySounds:
			EditorDraw::showSounds(!EditorDraw::getShowSounds());
            {
                static bbool prevObjectAABBs = btrue;

                if(EditorDraw::getShowSounds())
                {
                    prevObjectAABBs = EditorDraw::getShowActorAABBs();
                    EditorDraw::showActorAABBs(bfalse);
                }
                else
                {
                    // restore previous state
                    EditorDraw::showActorAABBs(prevObjectAABBs);
                }
            }
			break;

		case WwiseTopBarMenuID_Cmd_ShowMetronome:
			m_showMask ^= SHOW_METRONOME;
			break;

		case WwiseTopBarMenuID_Cmd_ShowEffects:
			m_showMask ^= SHOW_EFFECTS;
			break;

		case WwiseTopBarMenuID_Cmd_SoundInstances_ShowAll:
			m_showMask ^= SHOW_ALL_INSTANCES;
			if(m_showMask & SHOW_ALL_INSTANCES) m_showMask &= ~SHOW_SELECTED_INSTANCES;
			break;

		case WwiseTopBarMenuID_Cmd_SoundInstances_ShowOnlySelected:
			m_showMask ^= SHOW_SELECTED_INSTANCES;
			if(m_showMask & SHOW_SELECTED_INSTANCES) m_showMask &= ~SHOW_ALL_INSTANCES;
			break;

		default:
			return bfalse;
		}

        return btrue;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void WWISE_EditorPlugin::onDraw()
	{
		drawSoundInstances();
		if(m_showMask & SHOW_METRONOME) METRONOME_MANAGER->debugMetronomes();
		if(m_showMask & SHOW_EFFECTS)
			drawEffects();
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void WWISE_EditorPlugin::drawEffects()
	{

		AUDIO_ADAPTER->debugEffects();
	}
	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void WWISE_EditorPlugin::drawSoundInstances()
	{
		if((m_showMask & (SHOW_ALL_INSTANCES | SHOW_SELECTED_INSTANCES)) == 0) return;

		m_instancesMap.clear();

		AUDIO_ADAPTER->visitAllPlayingInstances(s_drawPlayingInstanceCallback, this);

		const Color color = Color::green();
		const u32	uDarkColor = ITFCOLOR_TO_U32(150, 50, 50, 50);
		const u32	uNormalColor = ITFCOLOR_TO_U32(150, 20, 20, 200);
		const f32	boxH = 16.f;
		const f32	charW = 7.f;

		for(InstancesMap::const_iterator it = m_instancesMap.begin(), itEnd = m_instancesMap.end(); it != itEnd; ++it)
		{
			const ITF_VECTOR<Instance>	&instanceList = it->second;

			Vec3d						pos3d = instanceList[0].pos;
			Vec3d						pos3dOut;

			GFX_ADAPTER->compute3DTo2D(pos3d, pos3dOut);

			Vec2d	vec2d(pos3dOut.x(), pos3dOut.y());
			vec2d += Vec2d(1.f, 1.f);

			String8 txt;
			f32		angle = 0.f;

			Vec2d	boxMin = vec2d;
			f32		xMax = vec2d.x(), yMax = vec2d.y();

			for(u32 i = 0; i < instanceList.size(); ++i)
			{
				if(instanceList[i].radius > 0.f)
				{
					const f32	radius = instanceList[i].radius;
					txt.setTextFormat("%s (%.3f m)", instanceList[i].soundName, radius);

					GFX_ADAPTER->drawDBGSphere(pos3d.getX(), pos3d.getY(), radius, color.getRed(), color.getGreen(), color.getBlue(), 0.f, pos3d.getZ(), 64, btrue, bfalse, bfalse, bfalse);

					Vec3d	pos3dbis = pos3d + Vec3d(radius * sin(angle), radius * cos(angle), 0.f);
					angle += 0.17f;

					GFX_ADAPTER->drawDBG3DLine(pos3d, pos3dbis, color);
				}
				else
				{
					txt.setTextFormat("%s", instanceList[i].soundName);
				}

				xMax = Max(xMax, vec2d.x() + Editor::fTextSubOffset + txt.getLen() * charW);
				yMax += boxH;

				GFX_ADAPTER->drawDBGText(txt, color, vec2d.x() + Editor::fTextSubOffset, vec2d.y());
				vec2d += Vec2d(0.f, boxH);
			}

			GFX_ADAPTER->drawDBG2dBox(boxMin, xMax - boxMin.x(), yMax - boxMin.y(), uNormalColor, uNormalColor, uDarkColor, uNormalColor);
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void WWISE_EditorPlugin::draw(const AudioPlayRequest &_playRequest)
	{
		const Wwise::AkGameObject * gao = _playRequest.m_objectRef.isValid() ? Wwise::AkGameObjectFactory::s_getSingleton()->getAkGameObject(_playRequest.m_objectRef) : NULL;
		if(!gao) return;

		const Color color = Color::green();
		const char	*soundName = AUDIO_ADAPTER->getWwiseNameFromID(_playRequest.m_soundEventID);

		if(m_showMask & SHOW_ALL_INSTANCES)
		{
		}
		else if(m_showMask & SHOW_SELECTED_INSTANCES)
		{
			const bbool isFocusObject = (EDITOR->getFocusObject() == _playRequest.m_objectRef);
			Actor		*actor = static_cast<Actor *>(GETOBJECT(_playRequest.m_objectRef));
			if(!actor || actor->isDestructionRequested()) return;

			const bbool showDetails = (EDITOR->getSelectionSize() < 5) && (isFocusObject || actor->isSelected());

			if(!showDetails) return;
		}
		else
		{
			return;
		}

		const char	*gaoName = gao->getName() ;
		Vec3d		pos = gao->getPosition() ;

		AkUInt32	numItem = 0;
		AKRESULT	res = AK_SoundEngine_Query_QueryAudioObjectIDs(_playRequest.m_soundEventID, numItem, NULL);
		if(res != AK_Success && res != AK_PartialSuccess) return;

		AkObjectInfo	*objectInfoArray = new AkObjectInfo[numItem];

		res = AK_SoundEngine_Query_QueryAudioObjectIDs(_playRequest.m_soundEventID, numItem, objectInfoArray);
		if(res == AK_Success)
		{
			for(AkUInt32 j = 0; j < numItem; ++j)
			{
				if(objectInfoArray[j].iDepth) continue;

				AkPositioningInfo	akPositioningInfo;
				res = AK_SoundEngine_Query_GetPositioningInfo(objectInfoArray[j].objID, akPositioningInfo);

				if(res == AK_Success)
				{
					Instance	instance;
					instance.gaoName = gaoName;
					instance.soundName = soundName;
					instance.pos = pos;

					if(akPositioningInfo.bUseAttenuation)
					{
						instance.radius = akPositioningInfo.fMaxDistance;
					}
					else
					{
						instance.radius = 0.f;
					}

					ITF_VECTOR<Instance>	&instances = m_instancesMap[(void *)gao];
					instances.push_back(instance);
				}
			}
		}

		if(objectInfoArray) delete[] objectInfoArray;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void WWISE_EditorPlugin::s_drawPlayingInstanceCallback(void *_this, const AudioPlayRequest &_playRq)
	{
		((WWISE_EditorPlugin *)_this)->draw(_playRq);
	}
}	// namespace ITF
#endif // ITF_FINAL
#endif
