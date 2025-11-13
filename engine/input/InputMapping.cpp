#include "precompiled_engine.h"

#ifndef _ITF_INPUT_MAPPING_H_
#include "engine/input/InputMapping.h"
#endif //_ITF_INPUT_MAPPING_H_

namespace ITF
{
    InputMapping::InputMapping()
    {
    }

    void InputMapping::AddMapping(const PhysicalInput& physical, const VirtualInput& virtualInput)
    {
        RemoveMapping(physical);

        m_physicalToVirtual[physical] = virtualInput;
        UpdateReverseMapping(physical, virtualInput, true);
    }

    void InputMapping::RemoveMapping(const PhysicalInput& physical)
    {
        auto it = m_physicalToVirtual.find(physical);
        if (it != m_physicalToVirtual.end())
        {
            VirtualInput oldVirtual = it->second;
            m_physicalToVirtual.erase(it);
            UpdateReverseMapping(physical, oldVirtual, false);
        }
    }

    bool InputMapping::GetVirtualInput(const PhysicalInput& physical, VirtualInput& virtualInput) const
    {
        auto it = m_physicalToVirtual.find(physical);
        if (it != m_physicalToVirtual.end())
        {
            virtualInput = it->second;
            return true;
        }
        return false;
    }

    u32 InputMapping::GetPhysicalInputs(const VirtualInput& virtualInput, std::vector<PhysicalInput>& physicalInputs) const
    {
        physicalInputs.clear();
        auto it = m_virtualToPhysical.find(virtualInput);
        if (it != m_virtualToPhysical.end())
        {
            physicalInputs = it->second;
            return static_cast<u32>(physicalInputs.size());
        }
        return 0;
    }

    bool InputMapping::HasMapping(const PhysicalInput& physical) const
    {
        return m_physicalToVirtual.find(physical) != m_physicalToVirtual.end();
    }

    void InputMapping::Clear()
    {
        m_physicalToVirtual.clear();
        m_virtualToPhysical.clear();
    }

    void InputMapping::AddMultipleMappings(const std::vector<PhysicalInput>& physicalInputs, const VirtualInput& virtualInput)
    {
        for (const auto& physical : physicalInputs)
        {
            AddMapping(physical, virtualInput);
        }
    }

    void InputMapping::UpdateReverseMapping(const PhysicalInput& physical, const VirtualInput& virtualInput, bool add)
    {
        if (add)
        {
            auto& physicalList = m_virtualToPhysical[virtualInput];
            bool found = false;
            for (const auto& p : physicalList)
            {
                if (p == physical)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                physicalList.push_back(physical);
            }
        }
        else
        {
            auto it = m_virtualToPhysical.find(virtualInput);
            if (it != m_virtualToPhysical.end())
            {
                auto& physicalList = it->second;
                for (auto listIt = physicalList.begin(); listIt != physicalList.end(); ++listIt)
                {
                    if (*listIt == physical)
                    {
                        physicalList.erase(listIt);
                        break;
                    }
                }
                if (physicalList.empty())
                {
                    m_virtualToPhysical.erase(it);
                }
            }
        }
    }

} // namespace ITF

