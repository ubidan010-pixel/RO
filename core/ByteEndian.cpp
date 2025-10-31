#include "precompiled_core.h"

#include "core/ByteEndian.h"

namespace ITF
{

    bbool    Endian::m_isLittleEndian;
    Endian    Endian::m_instance; // used so that the constructor initializes m_isLittleEndian

} // namespace ITF

