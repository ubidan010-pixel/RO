#ifndef _ITF_FXKIT_H_
#define _ITF_FXKIT_H_

#ifndef _ITF_FEEDBACKFXMANAGER_H_
#include "engine/actors/managers/FeedbackFXManager.h"
#endif //_ITF_FEEDBACKFXMANAGER_H_

namespace ITF
{

    class FxKit
    {
        DECLARE_SERIALIZE()
    public:
        FxKit(){}
        ~FxKit(){}
        StringID getFx(StringID _input) const { ITF_MAP<StringID,StringID>::const_iterator it = m_fxMap.find(_input); if (it == m_fxMap.end()) return m_default; else return it->second; }

		const ITF_VECTOR<SoundDescriptor_Template *>&	getSDTemplates(){ return m_soundDescriptorTemplates; }
		const ITF_VECTOR<FxDescriptor_Template *>&		getFXTemplates(){ return m_FxDescriptorTemplates; }
		FeedbackFXManager::ActorMap&					getActors()		{ return m_actors; }
		void initialize(ResourceGroup * _grp);

	private:

        void fillMap();

        struct Fx
        {
            DECLARE_SERIALIZE()
            StringID m_input;
            StringID m_output;
        };

        StringID m_default;

        ITF_VECTOR<Fx> m_fxList;
        ITF_MAP<StringID,StringID> m_fxMap;

		ITF_VECTOR<SoundDescriptor_Template *>	m_soundDescriptorTemplates;
		ITF_VECTOR<FxDescriptor_Template *>		m_FxDescriptorTemplates;

		FeedbackFXManager::ActorMap		m_actors;
    };

}

#endif // _ITF_FXKIT_H_

