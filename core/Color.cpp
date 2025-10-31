#include "precompiled_core.h"

#ifndef _ITF_COLOR_H_
#include "core/Color.h"
#endif //_ITF_COLOR_H_

#ifndef _ITF_ARCHIVE_H_
#include "core/Archive.h"
#endif //_ITF_ARCHIVE_H_

namespace ITF
{

void Color::serialize(ArchiveMemory& _archive)
{
    _archive.serialize(m_b);
    _archive.serialize(m_g);
    _archive.serialize(m_r);
    _archive.serialize(m_a);
}

}