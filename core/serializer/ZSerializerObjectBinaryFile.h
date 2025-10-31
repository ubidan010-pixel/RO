#ifndef SERIALIZEROBJECTBINARYFILE_H
#define SERIALIZEROBJECTBINARYFILE_H

#include "ZSerializerObjectBinary.h"
namespace ITF
{

class CSerializerObjectBinaryFile : public CSerializerObjectBinary
{
public:

    CSerializerObjectBinaryFile();

    bbool open( const Path& _file, bbool _read );
    bbool open( const Path& _file, const String& _szPlatform, bbool _read );
    
private:
    bool open( const String& cookedName, bbool _read );
};
} // namespace ITF

#endif // SERIALIZEROBJECT_H


