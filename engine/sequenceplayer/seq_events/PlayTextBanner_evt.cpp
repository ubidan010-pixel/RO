#include "precompiled_engine.h"

#ifndef _ITF_PLAYTEXTBANNER_EVT_H_
#include "engine/sequenceplayer/seq_events/PlayTextBanner_evt.h"
#endif //_ITF_PLAYTEXTBANNER_EVT_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

#ifndef _LOCALISATION_ID_H_
#include "core/LocalisationId.h"
#endif // _LOCALISATION_ID_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_



namespace ITF
{

u32	PlayTextBanner_evt::m_lastLineId;
u32	PlayTextBanner_evt::m_playState;

IMPLEMENT_OBJECT_RTTI(PlayTextBanner_evt)

PlayTextBanner_evt::PlayTextBanner_evt()
{
}

SequenceEvent * PlayTextBanner_evt::create(SequencePlayerComponent* _owner, SequenceEvent_Template *_template)
{
    PlayTextBanner_evt *pNew    = new PlayTextBanner_evt();
    pNew->m_ownerSequence   = _owner;
    pNew->m_template        = _template;
    return pNew;
}

void PlayTextBanner_evt::init(const Vec3d& _pos)
{
}

void PlayTextBanner_evt::forceCurrentFrame(   i32 _frame  )
{
    PlayTextBanner_evtTemplate *evtTemplate = getTemplate();
    ITF_UNUSED(evtTemplate);
    ITF_ASSERT(_frame >= (evtTemplate->getStartFrame() + evtTemplate->getOffset()) && _frame <= (evtTemplate->getStopFrame() + evtTemplate->getOffset()));
}


LocalisationId PlayTextBanner_evt::getLineId()
{
	u32 nbValidItem = 0;
	LocalisationId returnLine;

	switch(getTemplate()->getRandomMode())
	{
	case 0 :	// none	
		return getTemplate()->getLineId(0);

	case 1 :	// noRepeat (random excluding last played lineId)
		{
			u32 lineIdNoRepeat[LINEID_MAX_ITEM - 1] = {0};
			u32 randomIndex = 0, j = 0;

			for (u32 i = 0; i < LINEID_MAX_ITEM; i++)
			{
				if (getTemplate()->getLineId(i) != 0)
					nbValidItem++;
			}

			if (m_lastLineId != 0)	// having a previous entry, feed noRepeat array
			{
				for (u32 i = 0; i < nbValidItem; i++)
				{
					if (getTemplate()->getLineId(i) != m_lastLineId)
					{
						lineIdNoRepeat[j] = getTemplate()->getLineId(i).value;
						j++;
					}
				}

				randomIndex = rand() % j;
				m_lastLineId = lineIdNoRepeat[randomIndex];	
			}
			else
			{
				u32 randomIndex = rand() % nbValidItem;
				m_lastLineId = getTemplate()->getLineId(randomIndex).value;
			}
			
			returnLine.value = m_lastLineId;
			return returnLine;
		}

	case 2 :	// full 
		{
			for (u32 i = 0; i < LINEID_MAX_ITEM; i++)
			{
				if (getTemplate()->getLineId(i) != 0)
					nbValidItem++;
			}

			u32 randomIndex = rand() % nbValidItem;
			return getTemplate()->getLineId(randomIndex);
		}
 
	case 3 :	// once (the array is randomized and each item is played one time only)
		{
			u32 noPlayedLineId[LINEID_MAX_ITEM] = {0};

			for (u32 i = 0; i < LINEID_MAX_ITEM; i++)
			{
				if (getTemplate()->getLineId(i).value != 0)
				{
					if (m_playState & (1 << getTemplate()->getLineId(i).value))
						continue;
					else
					{
						noPlayedLineId[nbValidItem] = getTemplate()->getLineId(i).value;
						nbValidItem++;
					}
				}
			}

			if (nbValidItem > 0)
			{
				u32 randomIndex = rand() % nbValidItem;
				m_playState |= (1 << noPlayedLineId[randomIndex]);	// flag as played
				returnLine.value = noPlayedLineId[randomIndex];
				return returnLine;
			}
			else
			{
				m_playState = 0; // each item has been played, reinit flags
				nbValidItem = 0;
				return getLineId();	// recompute
			}
		}
	}
	
	return LocalisationId::Invalid;
}

void PlayTextBanner_evt::onBecomeActive()
{
    Super::onBecomeActive();

    GAMEMANAGER->showTutorialText(getLineId());
}

void PlayTextBanner_evt::onBecomeInactive()
{
    Super::onBecomeInactive();

    GAMEMANAGER->hideTutorialText();
}


/////////////////////////////////////////////////////
/// TEMPLATE
/////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(PlayTextBanner_evtTemplate)

BEGIN_SERIALIZATION_CHILD(PlayTextBanner_evtTemplate)
SERIALIZE_MEMBER("LineId", m_lineId[0].value);
SERIALIZE_MEMBER("LineId2",m_lineId[1].value);
SERIALIZE_MEMBER("LineId3",m_lineId[2].value);
SERIALIZE_MEMBER("LineId4",m_lineId[3].value);
SERIALIZE_MEMBER("LineId5",m_lineId[4].value);
SERIALIZE_MEMBER("LineId6",m_lineId[5].value);
/*SERIALIZE_CONTAINER("LineId", m_lineId);*/

SERIALIZE_MEMBER("Volume",m_volume);
SERIALIZE_MEMBER("FadeIn",m_fadeIn);
SERIALIZE_MEMBER("FadeOut",m_fadeOut);
SERIALIZE_MEMBER("RandomMode",m_randomMode);
SERIALIZE_MEMBER("ForcePrefetch",m_forcePrefetch);

END_SERIALIZATION()

PlayTextBanner_evtTemplate::PlayTextBanner_evtTemplate()
: m_volume(0.f)
, m_fadeIn(0.f)
, m_fadeOut(0.f)
, m_randomMode(0)
, m_forcePrefetch(bfalse)
{
	LocalisationId defautId;
	defautId.value = 0;
	for (int i = 0; i < LINEID_MAX_ITEM; i++)
		m_lineId.insert(defautId, i);
}

#ifdef ITF_SUPPORT_BLOBEVENT

SequenceEvent_Template*   PlayTextBanner_evtTemplate::createFromBlob( Blob* _blob, SequencePlayerComponent_Template* _owner )
{
    PlayTextBanner_evtTemplate* pNew = new PlayTextBanner_evtTemplate();
    pNew->setOwnerSequence(_owner);
    pNew->fillFromBlob(_blob);

    return pNew;
}

void PlayTextBanner_evtTemplate::serializeToBlob (Blob* _blob)
{
    serializeCommonToBlob(_blob);

	for (int i = 0; i < LINEID_MAX_ITEM; i++)
		_blob->pushInt32(m_lineId[i].value);

    _blob->pushFloat32(m_volume);
    _blob->pushFloat32(m_fadeIn);
    _blob->pushFloat32(m_fadeOut);
	_blob->pushInt32(m_randomMode);
    _blob->pushBool(m_forcePrefetch);
}

void PlayTextBanner_evtTemplate::fillFromBlob(Blob* _blob)
{
    fillCommonFromBlob(_blob);

	for (int i = 0; i < LINEID_MAX_ITEM; i++)
		m_lineId[i].value = _blob->extractInt32();

    m_volume = _blob->extractFloat32();
    m_fadeIn = _blob->extractFloat32();
    m_fadeOut = _blob->extractFloat32();
	m_randomMode = _blob->extractInt32();
    m_forcePrefetch = _blob->extractBool();
}
#endif  //ITF_SUPPORT_BLOBEVENT

} // namespace ITF


