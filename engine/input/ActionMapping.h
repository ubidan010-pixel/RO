#ifndef _ITF_ACTION_MAPPING_H_
#define _ITF_ACTION_MAPPING_H_

#ifndef _ITF_INPUT_TYPES_H_
#include "engine/input/InputTypes.h"
#endif //_ITF_INPUT_TYPES_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_

#include <map>

namespace ITF
{
    class ActionMapping
    {
    public:
        ActionMapping();
        ~ActionMapping() = default;

        void SetActionMapping(u32 player, const VirtualInput& virtualInput, InputAdapter::ActionType action);
        bool GetAction(u32 player, const VirtualInput& virtualInput, InputAdapter::ActionType& action) const;
        void ClearPlayerMapping(u32 player);
        void Clear();
        bool HasMapping(u32 player, const VirtualInput& virtualInput) const;

    private:
        typedef std::map<VirtualInput, InputAdapter::ActionType> PlayerActionMap;
        std::map<u32, PlayerActionMap> m_playerActionMaps;
    };

} // namespace ITF

#endif //_ITF_ACTION_MAPPING_H_

