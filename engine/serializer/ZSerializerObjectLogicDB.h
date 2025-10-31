#ifndef SERIALIZEROBJECTLOGICDB_H
#define SERIALIZEROBJECTLOGICDB_H

#ifndef SERIALIZEROBJECT_H
#include "core/serializer/ZSerializerObject.h"
#endif //SERIALIZEROBJECT_H

namespace ITF
{
class LogicDataTable;

class CSerializerObjectLogicDB : public CSerializerObject
{
public:
    
    CSerializerObjectLogicDB():CSerializerObject(btrue), m_bIsValid(bfalse) {}
    CSerializerObjectLogicDB(const Path & _path);
    CSerializerObjectLogicDB(const Path & _path,const String& _platform);
    CSerializerObjectLogicDB(const String& _id, const String8 & _luaContent);
    virtual ~CSerializerObjectLogicDB() {}

    void init(const Path & _path,const String& _platform);

    virtual bbool OpenGroup( const char* tag, uSize index = 0 ) override;
    virtual void CloseGroup() override;

    virtual void Serialize( const char* tag, char& b ) override;
    virtual void Serialize( const char* tag, unsigned char& b ) override;
    virtual void Serialize( const char* tag, short& s ) override;
    virtual void Serialize( const char* tag, int& i ) override;
    virtual void Serialize( const char* tag, u32& i ) override;
    virtual void Serialize( const char* tag, float& i ) override;
    virtual void Serialize( const char* tag, double& i ) override {ITF_ASSERT(0);}
    virtual void Serialize( const char* tag, bool& b ) override {ITF_ASSERT(0);}
    virtual void Serialize( const char* tag, String& str ) override;
    virtual void Serialize( const char* tag, ITF_STDSTRING& str ) override;
    virtual void Serialize( const char* tag, class Vec2d& vec ) override;
    virtual void Serialize( const char* tag, class Vec3d& vec ) override;
    virtual void Serialize( const char* tag, ObjectPath& _path ) override;
    virtual void Serialize( const char* tag, class Color& _color) override;
    virtual void Serialize( const char* tag, class Angle& _angle ) override;
    virtual void Serialize( const char* tag, class Volume& _volume ) override;
    virtual void Serialize( const char* tag, class Path& _path ) override;
    virtual void Serialize( const char * tag, ObjectId & _id)  override { ITF_ASSERT(0); }
    virtual void Serialize( const char * tag, String8 & _id) override;
    virtual void Serialize( const char* tag, StringID & _id) override;
    virtual void Serialize( const char* tag, struct LocalisationId& _id ) override;

    virtual void SerializeEnumBegin( const char* tag, int& selection ) override { Serialize(tag,selection); }
    virtual void SerializeEnumVar( int var_num, const char* var ) override {};
    virtual void SerializeEnumEnd() override {};

    bbool  isValid() const {return m_bIsValid;}
    bbool  isEmpty() const;

    #if !defined (ITF_FINAL) && !defined (ITF_CONSOLE)
        static void     addHandledPathError(const Path& _path);
        static bbool    isPathErrorHandled(const Path& _path);
    #endif // !defined (ITF_FINAL) && !defined (ITF_CONSOLE)

    //keep for everything in case
    static void clear() 
    {
#if !defined (ITF_FINAL) && !defined (ITF_CONSOLE)
        m_alreadyHandledErrors.clear();
#endif
    
    }

protected:

    virtual bbool ReadArrayCount( const char* tag, uSize& count ) override;
    virtual bbool WriteArrayCount( const char* tag, uSize count ) override;

    SafeArray< const LogicDataTable* > m_tables;
private:
    bbool   m_bIsValid;
    #if !defined (ITF_FINAL) && !defined (ITF_CONSOLE)
        static ITF_VECTOR<Path> m_alreadyHandledErrors;
    #endif // !defined (ITF_FINAL) && !defined (ITF_CONSOLE)
};
} // namespace ITF

#endif // SERIALIZEROBJECTLOGICDB_H


