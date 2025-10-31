#ifndef _LOCALISATION_ID_H_
#define _LOCALISATION_ID_H_

#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_

namespace ITF
{
    struct LocalisationId
    {
        static const LocalisationId Invalid;

        LocalisationId(): value( U32_INVALID ) {}

        /************************************************************************/
        ITF_INLINE const LocalisationId& operator = (const LocalisationId& src)   {  value = src.value; return *this; }
        ITF_INLINE const LocalisationId& operator = (const u32& src           )   {  value = src; return *this;       }
        ITF_INLINE const LocalisationId& operator = (const i32& src           )   {  value = src; return *this;       }

        ITF_INLINE bool operator==(const LocalisationId& scr) const  {   return (value == scr.value);   }
        ITF_INLINE bool operator==(const u32& scr) const             {   return (value == scr);         }
        ITF_INLINE bool operator!=(const LocalisationId& scr) const  {   return (value != scr.value);   }
        ITF_INLINE bool operator!=(const u32& scr) const             {   return (value != scr);         }

        ITF_INLINE  bool operator < (const LocalisationId& _loc) const {return value < _loc.value;}
        /************************************************************************/

        ITF_INLINE void serialize(ArchiveMemory& _archive)
        {   
            _archive.serialize(value);
        }

        u32 value;  // LineId value
    };
}

#endif  //_LOCALISATION_ID_H_
