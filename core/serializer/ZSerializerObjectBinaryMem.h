#ifndef SERIALIZEROBJECTBINARYMEM_H
#define SERIALIZEROBJECTBINARYMEM_H

#include "ZSerializerObjectBinary.h"
namespace ITF
{

class CSerializerObjectBinaryMem : public CSerializerObjectBinary
{
public:

    CSerializerObjectBinaryMem();

    void Open( char* mem, uSize size, bbool read );

private:

    class CSerializerMem*   m_mem;
};
} // namespace ITF

#endif // SERIALIZEROBJECTBINARYMEM_H


