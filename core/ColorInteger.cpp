
#include "precompiled_core.h"

#ifndef _ITF_COLORINTEGER_H_
#include "core/ColorInteger.h"
#endif //_ITF_COLORINTEGER_H_

#ifndef _ITF_ARCHIVE_H_
#include "core/Archive.h"
#endif //_ITF_ARCHIVE_H_

namespace ITF
{

void ColorInteger::serialize(ArchiveMemory& _archive)
{
    if(_archive.isReading())
    {
        _archive.serialize(m_color);
        m_color = ITFCOLOR_FROM_ARGB(m_color);
    }
    else
    {
        u32 write_color = ITFCOLOR_TO_ARGB(m_color);
        _archive.serialize(write_color);
    }
}

}