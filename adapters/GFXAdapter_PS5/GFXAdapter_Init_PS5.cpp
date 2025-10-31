#include "precompiled_GFXAdapter_PS5.h"

#include "GFXAdapter_PS5.h"

#include "core/memory/MemMng/MemMngPage_PS5.h"
#include "Label_PS5.h"
#include "VRAMAllocator/VRAMAllocator.h"
#include "GFXAdapter_struct_PS5.h"
#include "ShaderProgram_PS5.h"


namespace ITF
{
    static constexpr uPtr GFX_MEMORY_BIG_PAGESIZE = 64ul * 1024ul;

    void GFXAdapter_PS5::initMemoryManagement()
    {
        MemoryPageManager::InitParams pageMgrParam;
        pageMgrParam.setReserveVirtualSpace(bfalse); // we let the mgr deal with addresses

        m_memorySmallPageMgrDirectMemory.Init(pageMgrParam);

        pageMgrParam.setPageAlignment(Max<uPtr>(MemoryPageManager::VIRTUAL_SPACE_PAGE_MINIMAL_ALIGNMENT, GFX_MEMORY_BIG_PAGESIZE));
        m_memoryBigPageMgrDirectMemory.Init(pageMgrParam);

        uPtr labelSize = 2048 * sizeof(Gnmp::LabelManager::PlatformLabel);
        u8* labelData = m_memorySmallPageMgrDirectMemory.AllocArea(labelSize, MemoryPageManager::Protection::allRW(), sizeof(u64));
        new Gnmp::LabelManager(labelData, labelSize);

        // VRAM allocator
        Gnmp::VRAMAllocator::init(m_memoryBigPageMgrDirectMemory);
    }

    void GFXAdapter_PS5::initDebugResourceRegistration()
    {
#if defined(ITF_ENABLE_AGC_RESOURCE_REGISTERING) && ITF_ENABLE_AGC_RESOURCE_REGISTERING
        size_t resourceRegistrationBufferSize;
        SceError error = sce::Agc::ResourceRegistration::queryMemoryRequirements(&resourceRegistrationBufferSize, Gnmp::AGC_RESOURCE_REGISTERING_MAX_NUM_OWNERS, Gnmp::AGC_RESOURCE_REGISTERING_MAX_STRING_LENGTH);
        if (error != SCE_AGC_ERROR_RESOURCE_REGISTRATION_NO_PA_DEBUG)
        {
            SCE_AGC_ASSERT(error == SCE_OK);
            u8* registrationBuffer = m_memoryBigPageMgrDirectMemory.AllocArea(resourceRegistrationBufferSize, MemoryPageManager::Protection::allRW(), sce::Agc::Alignment::kResourceRegistration);
            error = sce::Agc::ResourceRegistration::init(registrationBuffer, resourceRegistrationBufferSize, Gnmp::AGC_RESOURCE_REGISTERING_MAX_STRING_LENGTH);
            SCE_AGC_ASSERT(error == SCE_OK);
            error = sce::Agc::ResourceRegistration::registerDefaultOwner(nullptr);
            SCE_AGC_ASSERT(error == SCE_OK);
        }
#endif // ITF_ENABLE_AGC_RESOURCE_REGISTERING
    }

    GFXAdapter_PS5::GFXAdapter_PS5()
    {
        ITF_VERIFY_SCE_CALL(sce::Agc::init());
        initMemoryManagement();
        initDebugResourceRegistration();
    }

    GFXAdapter_PS5::~GFXAdapter_PS5()
    {
    }

    void GFXAdapter_PS5::init()
    {
        createShaderParametersDB();

        loadCoreShaders();

        initDefaultRenderStates();

        // Create IB and VB for dynamic usage
        createInternalBuffers();

        initAfterFx();
    }

    void GFXAdapter_PS5::destroy()
    {
        GFXAdapter::destroy();
        // destroy of the PS5 GFX adapter is never supposed to be called (the process is killed by the OS)
        NOT_IMPLEMENTED_ON_PS5
    }

    void GFXAdapter_PS5::destroyResources()
    {
        GFXAdapter::destroyResources();
        // As destroy, should never be called
        NOT_IMPLEMENTED_ON_PS5
    }

    void GFXAdapter_PS5::initDefaultRenderStates()
    {
        // ?
    }

} // namespace ITF
