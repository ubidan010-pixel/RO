#ifndef _ITF_INPUT_MAPPING_DEFAULTS_H_
#define _ITF_INPUT_MAPPING_DEFAULTS_H_

#ifndef _ITF_INPUT_MAPPING_H_
#include "engine/input/InputMapping.h"
#endif //_ITF_INPUT_MAPPING_H_

#ifndef _ITF_ACTION_MAPPING_H_
#include "engine/input/ActionMapping.h"
#endif //_ITF_ACTION_MAPPING_H_

namespace ITF
{
    void InitializeDefaultKeyboardMappings(InputMapping& mapping);
    void InitializeDefaultControllerMappings(InputMapping& mapping, u32 deviceId);
    void InitializeDefaultActionMappings(ActionMapping& actionMapping);

} // namespace ITF

#endif //_ITF_INPUT_MAPPING_DEFAULTS_H_

