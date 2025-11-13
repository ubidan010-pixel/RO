#include "precompiled_engine.h"

#ifndef _ITF_ACTION_MAPPING_H_
#include "engine/input/ActionMapping.h"
#endif //_ITF_ACTION_MAPPING_H_

namespace ITF
{
    ActionMapping::ActionMapping()
    {
    }

    void ActionMapping::SetActionMapping(u32 player, const VirtualInput& virtualInput, InputAdapter::ActionType action)
    {
        ITF_ASSERT(player < JOY_MAX_COUNT);
        if (player >= JOY_MAX_COUNT)
            return;

        m_playerActionMaps[player][virtualInput] = action;
    }

    bool ActionMapping::GetAction(u32 player, const VirtualInput& virtualInput, InputAdapter::ActionType& action) const
    {
        if (player >= JOY_MAX_COUNT)
            return false;

        auto playerIt = m_playerActionMaps.find(player);
        if (playerIt == m_playerActionMaps.end())
            return false;

        const PlayerActionMap& playerMap = playerIt->second;
        auto actionIt = playerMap.find(virtualInput);
        if (actionIt == playerMap.end())
            return false;

        action = actionIt->second;
        return true;
    }

    void ActionMapping::ClearPlayerMapping(u32 player)
    {
        if (player >= JOY_MAX_COUNT)
            return;

        auto it = m_playerActionMaps.find(player);
        if (it != m_playerActionMaps.end())
        {
            m_playerActionMaps.erase(it);
        }
    }

    void ActionMapping::Clear()
    {
        m_playerActionMaps.clear();
    }

    bool ActionMapping::HasMapping(u32 player, const VirtualInput& virtualInput) const
    {
        if (player >= JOY_MAX_COUNT)
            return false;

        auto playerIt = m_playerActionMaps.find(player);
        if (playerIt == m_playerActionMaps.end())
            return false;

        const PlayerActionMap& playerMap = playerIt->second;
        return playerMap.find(virtualInput) != playerMap.end();
    }

} // namespace ITF

