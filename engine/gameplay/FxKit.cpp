
#include "precompiled_engine.h"

#ifndef _ITF_FXKIT_H_
#include "engine/gameplay/FxKit.h"
#endif //_ITF_FXKIT_H_


namespace ITF
{

    BEGIN_SERIALIZATION(FxKit)
        SERIALIZE_CONTAINER_OBJECT("data",m_fxList);
        SERIALIZE_MEMBER("default",m_default);
        SERIALIZE_FUNCTION(fillMap,ESerializeGroup_DataEditable);
		SERIALIZE_CONTAINER_OBJECT("soundDescriptors",m_soundDescriptorTemplates);
		SERIALIZE_CONTAINER_OBJECT("FxDescriptors",m_FxDescriptorTemplates);
		SERIALIZE_CONTAINER_OBJECT("actors",m_actors);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(FxKit,Fx)
        SERIALIZE_MEMBER("input",m_input);
        SERIALIZE_MEMBER("output",m_output);
    END_SERIALIZATION()

    void FxKit::fillMap()
    {
        for (ITF_VECTOR<Fx>::const_iterator it = m_fxList.begin(); it != m_fxList.end(); ++it)
        {
            m_fxMap[it->m_input] = it->m_output;
        }
    }

	void FxKit::initialize(ResourceGroup * _grp)
	{
		for (u32 i = 0 ; i < m_soundDescriptorTemplates.size() ; ++i)
		{
			m_soundDescriptorTemplates[i]->addToResourceGroup(_grp);
		}

		for (u32 i = 0 ; i < m_FxDescriptorTemplates.size() ; ++i)
		{
			m_FxDescriptorTemplates[i]->addToResourceGroup(_grp);
		}

	}
}
