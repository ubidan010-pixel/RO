#include "precompiled_core.h"
#include "ZSerializerObjectBinaryMem.h"
#include "ZSerializerMem.h"
namespace ITF
{

CSerializerObjectBinaryMem::CSerializerObjectBinaryMem() : CSerializerObjectBinary()
{
    m_serializer    = NULL;
    m_mem           = NULL;
}

void CSerializerObjectBinaryMem::Open( char* mem, uSize size, bbool read )
{
    m_read = read;

    if ( read )
        m_mem->OpenRead(mem);
    else
        m_mem->OpenWrite(mem);
}

} // namespace ITF
