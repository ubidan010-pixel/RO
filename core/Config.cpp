#include "precompiled_core.h"

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

namespace ITF
{
    const CharacterAnim* CharacterConfig::findAction(const String& _action) const
    {
        ITF_VECTOR<CharacterAnim>::const_iterator it = m_animsConfig.begin();
        ITF_VECTOR<CharacterAnim>::const_iterator end = m_animsConfig.end();

        for (; it != end; ++it)
            if ((*it).m_action == _action)
                return &(*it);

        return NULL;
    }
} // namespace ITF


