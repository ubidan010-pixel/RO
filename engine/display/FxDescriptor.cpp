
#include "precompiled_engine.h"

#ifndef _ITF_FXDESCRIPTOR_H_
#include "engine/display/FxDescriptor.h"
#endif //_ITF_FXDESCRIPTOR_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

namespace ITF
{
	///////////////////////////////////////////////////////////////////////////////////////////
	BEGIN_SERIALIZATION(FxDescriptor_Template)
		SERIALIZE_MEMBER("name",m_name);
		SERIALIZE_OBJECT("gen",m_gen);
		SERIALIZE_MEMBER("texture",m_textureFile);
		SERIALIZE_MEMBER("angleOffset",m_angleOffset);
		SERIALIZE_MEMBER("minDelay",m_minDelay);
		SERIALIZE_MEMBER("maxDelay",m_maxDelay);
        SERIALIZE_OBJECT("frequencyInput",m_frequencyInput);
        SERIALIZE_OBJECT("emitCountInput",m_emitCountInput);
        SERIALIZE_OBJECT("velocityInput",m_velocityInput);
        SERIALIZE_OBJECT("velocityDeltaInput",m_velocityDeltaInput);
        SERIALIZE_OBJECT("angularSpeedInput",m_angularSpeedInput);
        SERIALIZE_OBJECT("angularSpeedDeltaInput",m_angularSpeedDeltaInput);
        SERIALIZE_MEMBER("draw2D", m_draw2D);
	END_SERIALIZATION()

	FxDescriptor_Template::FxDescriptor_Template()
		: m_angleOffset(0.0f)
		, m_minDelay(0.0f)
		, m_maxDelay(0.0f)
        , m_draw2D(bfalse)
	{
        m_resource.invalidateResourceId();
	}

	FxDescriptor_Template::~FxDescriptor_Template()
	{
        RESOURCE_MANAGER->queryRemoveUserToResourceLogicalData(m_resource);
        m_resource.invalidateResourceId();
	}

    void FxDescriptor_Template::onTemplateLoaded()
    {
        m_resource = RESOURCE_MANAGER->newResourceIDFromFile(Resource::ResourceType_Texture,m_textureFile);
    }

	void FxDescriptor_Template::addToResourceGroup(ResourceGroup* _resourceGroup)
	{
		ITF_ASSERT_MSG(_resourceGroup, "FxDescriptor needs a valid resource group !");
		if (_resourceGroup)
		{
			_resourceGroup->addResource(m_resource.getResource());
		}
	}

    void FxDescriptor_Template::removeFromResourceGroup(ResourceGroup* _resourceGroup)
    {
        ITF_ASSERT_MSG(_resourceGroup, "FxDescriptor needs a valid resource group !");
        if (_resourceGroup && m_resource.isValidResourceId() )
        {
            _resourceGroup->removeResource(m_resource);
        }
    }

	FxDescriptor::FxDescriptor()
		: m_template(NULL)
	{
	}

	void FxDescriptor::onActorLoaded( const FxDescriptor_Template* _template )
	{
		m_template = _template;
        
        ITF_ASSERT(m_template!=NULL);
	}

	FxDescriptor::~FxDescriptor()
	{
	}

	void FxDescriptor::clear()
	{
		m_template = NULL;
	}
}