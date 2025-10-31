#ifndef SERIALIZEROBJECTBINARY_H
#define SERIALIZEROBJECTBINARY_H

#include "core/serializer/ZSerializerObject.h"

namespace ITF
{
class ArchiveMemory;

class CSerializerObjectBinary : public CSerializerObject
{
public:
    
    CSerializerObjectBinary() : m_serializer(NULL),m_bDestroySerializer(bfalse) { m_properties |= ESerializerProperty_Binary; }
    CSerializerObjectBinary(ArchiveMemory* _Archive,bbool _bDestroySerializer = bfalse);
    virtual ~CSerializerObjectBinary();

    void        Init( ArchiveMemory* _Archive );
    virtual bbool OpenGroup( const char* tag, uSize index ) override { return true; }
    virtual void CloseGroup() override {}
    void        rewindForReading();

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
    virtual void Serialize( const char* tag, ObjectId & _id) override;
    virtual void Serialize( const char* tag, StringID & _id) override;
    virtual void Serialize( const char* tag, struct LocalisationId& _id ) override;

    virtual void SerializeEnumBegin( const char* tag, int& selection ) override;
    virtual void SerializeEnumVar( int var_num, const char* var ) override {}
    virtual void SerializeEnumEnd() override {}

    bbool isValid() {return (m_serializer!=NULL);};

protected:

    virtual bbool ReadArrayCount( const char* tag, uSize& count ) override;
    virtual bbool WriteArrayCount( const char* tag, uSize count ) override;

    class ArchiveMemory*  m_serializer;
    bbool m_bDestroySerializer;
};

template <class T>
void BinaryClone(T *object, T *dest, ESerializeFlags _saveFlag, ESerializeFlags _loadFlag)
{
    ArchiveMemory archiveMem(sizeof(T), 0, bfalse);
    CSerializerObjectBinary serializeBin(&archiveMem);
    object->Serialize(&serializeBin, _saveFlag);
    
    serializeBin.rewindForReading();
    dest->Serialize(&serializeBin,_loadFlag);
}


template <class T>
void BinaryClone(T *object, T *dest)
{
    BinaryClone(object, dest, ESerialize_Data_Save, ESerialize_Data_Load);
}

template <class T>
void BaseObjectClone(T *object, T *dest)
{
    BinaryClone(object, dest);
    dest->regenerateObjectId();
}

} // namespace ITF

#endif // SERIALIZEROBJECT_H


