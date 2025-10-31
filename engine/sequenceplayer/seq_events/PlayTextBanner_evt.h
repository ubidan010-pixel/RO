
#ifndef _ITF_PLAYTEXTBANNER_EVT_H_
#define _ITF_PLAYTEXTBANNER_EVT_H_


#ifndef _ITF_SEQUENCEPLAYERCOMPONENT_H_
#include "engine/sequenceplayer/SequencePlayerComponent.h"
#endif //_ITF_SEQUENCEPLAYERCOMPONENT_H_

#ifndef _ITF_SIMPLETEXTCOMPONENT_H_
#include "gameplay/components/UI/SimpleTextComponent.h"
#endif //_ITF_SIMPLETEXTCOMPONENT_H_

namespace ITF
{
	#define LINEID_MAX_ITEM		6

	class Blob;
	class PlayTextBanner_evtTemplate;

	class PlayTextBanner_evt : public SequenceEvent
	{
		DECLARE_OBJECT_CHILD_RTTI(PlayTextBanner_evt,SequenceEvent,2330070311);

	public:

		PlayTextBanner_evt              (   );
		virtual ~PlayTextBanner_evt     (   ) {};

		static SequenceEvent * create(SequencePlayerComponent* _owner, SequenceEvent_Template *_template);

		void                    init                    (  const Vec3d& _pos );
		void                    forceCurrentFrame       (   i32 _frame       );
		void                    onBecomeActive          (                    ); 
		void                    onBecomeInactive        (                    );
		int						getRandomizedLineId		(					 );

		PlayTextBanner_evtTemplate *getTemplate() { return (PlayTextBanner_evtTemplate *)m_template; }

	private :
		LocalisationId			getLineId();
		static u32				m_lastLineId;
		static u32				m_playState;
	};


	class PlayTextBanner_evtTemplate : public SequenceEvent_Template
	{
	public:
		DECLARE_SERIALIZE()
		DECLARE_OBJECT_CHILD_RTTI(PlayTextBanner_evtTemplate,SequenceEvent_Template,304365072);

		PlayTextBanner_evtTemplate              (           );
		virtual ~PlayTextBanner_evtTemplate     (           ) { };
		EvtType getType                    (           ) const  { return event_PlayTextBanner ;}

#ifdef ITF_SUPPORT_BLOBEVENT
		static  SequenceEvent_Template* createFromBlob  (Blob* _blob, SequencePlayerComponent_Template* _owner );
		virtual     void                serializeToBlob (Blob* _blob);
		virtual     void                fillFromBlob    (Blob* _blob);
#endif // ITF_SUPPORT_BLOBEVENT



		const LocalisationId	getLineId(u32 _index = 0) { return m_lineId[_index]; }
		const f32               getVolume() { return m_volume; }
		const f32               getFadeIn() { return m_fadeIn; }
		const f32               getFadeOut() { return m_fadeOut; }
		const u32				getRandomMode() { return m_randomMode; }
        const bbool             getForcePrefetch() { return m_forcePrefetch; }

	private:
		SafeArray<LocalisationId, LINEID_MAX_ITEM>	m_lineId;
		f32                     m_volume;
		f32                     m_fadeIn;
		f32                     m_fadeOut;
		u32						m_randomMode;
        bbool                   m_forcePrefetch;
	};
} // namespace ITF

#endif // _ITF_PLAYTEXTBANNER_EVT_H_

