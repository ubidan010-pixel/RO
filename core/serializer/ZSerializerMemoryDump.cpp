#include "precompiled_core.h"

#ifndef SERIALIZERMEMORYDUMP_H
#include "core/serializer/ZSerializerMemoryDump.h"
#endif // SERIALIZERMEMORYDUMP_H

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_COLOR_H_
#include "core/Color.h"
#endif //_ITF_COLOR_H_

#ifndef _ITF_OBJECTPATH_H_
#include "core/ObjectPath.h"
#endif //_ITF_OBJECTPATH_H_

#ifndef _ITF_ARCHIVECOUNTER_H_
#include "core/ArchiveCounter.h"
#endif //_ITF_ARCHIVECOUNTER_H_


#ifdef ITF_IPAD
#include <string>
#endif

namespace ITF
{

CSerializerMemoryDump::CSerializerMemoryDump()
{
	m_serializer = new ArchiveCounter();
	m_properties |= ESerializerProperty_Binary;
}

CSerializerMemoryDump::~CSerializerMemoryDump()
{
    SF_DEL(m_serializer);
}


void CSerializerMemoryDump::Serialize( const char* tag, char& b )
{
	m_serializer->serialize((char&)b);
}

void CSerializerMemoryDump::Serialize( const char* tag, unsigned char& b )
{
	m_serializer->serialize((unsigned char&)b);
}

void CSerializerMemoryDump::Serialize( const char* tag, short& s )
{
	m_serializer->serialize((short&)s);
}

void CSerializerMemoryDump::Serialize( const char* tag, int& i )
{
	m_serializer->serialize((int&)i);
}

void CSerializerMemoryDump::Serialize( const char* tag, u32& i )
{
	m_serializer->serialize((u32&)i);
}

void CSerializerMemoryDump::Serialize( const char* tag, float& f )
{
	m_serializer->serialize((float&)f);
}

void CSerializerMemoryDump::Serialize( const char* tag, double& d )
{
	m_serializer->serialize((double&)d);
}

void CSerializerMemoryDump::Serialize( const char* tag, bool& b )
{
	m_serializer->serialize((bool&)b);
}

void CSerializerMemoryDump::Serialize( const char* tag, String & str )
{
	str.serialize(*m_serializer);
}

void CSerializerMemoryDump::Serialize( const char * tag, ObjectId & _id)
{
	m_serializer->serialize((u64&)_id.m_id);
}

void CSerializerMemoryDump::Serialize( const char * tag, StringID & _id)
{
	u32 size = sizeof(StringID::StringIdValueType);
	m_serializer->serialize(size);//test;
}

void CSerializerMemoryDump::Serialize( const char* tag, String8& str )
{
	str.serialize(*m_serializer);
}

void CSerializerMemoryDump::Serialize( const char* tag, ITF_STDSTRING & _str)
{
	String8 buf;

	if (!m_serializer->isReading())
	{
		buf = _str.c_str();
	}
	Serialize(tag,buf);
}

void CSerializerMemoryDump::Serialize( const char* tag, Vec2d& vec )
{
	vec.serialize(*m_serializer);
}

void CSerializerMemoryDump::Serialize( const char* tag, Vec3d& vec )
{
	vec.serialize(*m_serializer);
}

void CSerializerMemoryDump::Serialize( const char* tag, ObjectPath& _path )
{
	ITF_STDSTRING path;

	{
		_path.toString(path);
		Serialize(tag,path);
	}
}

void CSerializerMemoryDump::Serialize( const char* tag, Color& _color )
{
	_color.serialize(*m_serializer);
}

void CSerializerMemoryDump::Serialize( const char* tag, Angle& _angle )
{
	_angle.serialize(*m_serializer);
}

void CSerializerMemoryDump::Serialize( const char* tag, Path& _path )
{
	_path.serialize(*m_serializer);
}


void CSerializerMemoryDump::Serialize( const char* tag, Volume& _volume)
{
	_volume.serialize(*m_serializer);
}

void CSerializerMemoryDump::SerializeEnumBegin( const char* tag, int& selection )
{
	m_serializer->serialize((i32&)selection);
}

bbool CSerializerMemoryDump::ReadArrayCount( const char* tag, uSize& count )
{
	m_serializer->serialize(count);
	return btrue;
}

bbool CSerializerMemoryDump::WriteArrayCount( const char* tag, uSize count )
{
	m_serializer->serialize(count);
	return btrue;
}

} // namespace ITF
