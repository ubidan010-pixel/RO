#ifndef SERIALIZERMEMORYDUMP_H
#define SERIALIZERMEMORYDUMP_H

#ifndef SERIALIZEROBJECT_H
#include "core/serializer/ZSerializerObject.h"
#endif

class TiXmlDocument;
class TiXmlElement;

namespace ITF
{

class CSerializerMemoryDump : public CSerializerObject
{
public:

	CSerializerMemoryDump();
    virtual ~CSerializerMemoryDump();

    virtual bbool OpenGroup( const char* tag, uSize index ) override { return true; }
    virtual void CloseGroup() override {}

	virtual void Serialize( const char* tag, char& b ) override;
	virtual void Serialize( const char* tag, unsigned char& b ) override;
	virtual void Serialize( const char* tag, short& s ) override;
	virtual void Serialize( const char* tag, int& i ) override;
	virtual void Serialize( const char* tag, u32& ui ) override;
	virtual void Serialize( const char* tag, float& i ) override;
	virtual void Serialize( const char* tag, double& i ) override;
	virtual void Serialize( const char* tag, bool& b ) override;
	virtual void Serialize( const char* tag, String & str ) override;
	virtual void Serialize( const char* tag, String8& str ) override;
	virtual void Serialize( const char* tag, ITF_STDSTRING& str ) override;
	virtual void Serialize( const char* tag, class Vec2d& vec ) override;
	virtual void Serialize( const char* tag, class Vec3d& vec ) override;
	virtual void Serialize( const char* tag, class ObjectPath& _objPath ) override;
	virtual void Serialize( const char* tag, class Color& _color ) override;
	virtual void Serialize( const char* tag, class Angle& _angle ) override;
	virtual void Serialize( const char* tag, class Volume& _volume) override;
	virtual void Serialize( const char* tag, class Path& _path ) override;
	virtual void Serialize( const char * tag, ObjectId & _id) override;
	virtual void Serialize( const char* tag, StringID & _id) override;

	virtual void SerializeEnumBegin( const char* tag, int& selection ) override;
	virtual void SerializeEnumVar( int var_num, const char* var ) override {}
	virtual void SerializeEnumEnd() override {}

protected:

    virtual bbool ReadArrayCount( const char* tag, uSize& count ) override;
    virtual bbool WriteArrayCount( const char* tag, uSize count ) override;

	class ArchiveMemory*  m_serializer;
};


} // namespace ITF

#endif // SERIALIZERMEMORYDUMP_H


