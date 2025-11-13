#ifndef _ITF_INPUT_MAPPING_H_
#define _ITF_INPUT_MAPPING_H_

#ifndef _ITF_INPUT_TYPES_H_
#include "engine/input/InputTypes.h"
#endif //_ITF_INPUT_TYPES_H_

#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_

#include <map>
#include <vector>

namespace ITF
{
    class InputMapping
    {
    public:
        InputMapping();
        ~InputMapping() = default;
        void AddMapping(const PhysicalInput& physical, const VirtualInput& virtualInput);

        void RemoveMapping(const PhysicalInput& physical);

        bool GetVirtualInput(const PhysicalInput& physical, VirtualInput& virtualInput) const;

        u32 GetPhysicalInputs(const VirtualInput& virtualInput, std::vector<PhysicalInput>& physicalInputs) const;

        bool HasMapping(const PhysicalInput& physical) const;

        void Clear();

        u32 GetMappingCount() const { return static_cast<u32>(m_physicalToVirtual.size()); }

        void AddMultipleMappings(const std::vector<PhysicalInput>& physicalInputs, const VirtualInput& virtualInput);

    private:
        std::map<PhysicalInput, VirtualInput> m_physicalToVirtual;
        std::map<VirtualInput, std::vector<PhysicalInput>> m_virtualToPhysical;

        void UpdateReverseMapping(const PhysicalInput& physical, const VirtualInput& virtualInput, bool add);
    };

} // namespace ITF

#endif //_ITF_INPUT_MAPPING_H_

