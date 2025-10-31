#include "precompiled_core.h"
//
//#ifndef _ITF_ZSERIALIZER_SCENE_DEP_H_
//#include "core/serializer/ZSerializerSceneDep.h"
//#endif //_ITF_ZSERIALIZER_SCENE_DEP_H_
//
//#ifndef _ITF_LOCALISATIONMANAGER_H_
//#include "engine/localisation/LocalisationManager.h"
//#endif //_ITF_LOCALISATIONMANAGER_H_
//
//#ifndef _LOCALISATION_ID_H_
//#include "core/LocalisationId.h"
//#endif //_LOCALISATION_ID_H_
//
//namespace ITF
//{
//
//CSerializerSceneDep::CSerializerSceneDep()
//: m_internalSerializer( NULL )
//, m_pathTags( NULL )
//{
//}
//
//void CSerializerSceneDep::setSerializer( CSerializerObject* serializer )
//{
//    ITF_ASSERT_CRASH( serializer != NULL, "Invalid internal serializer" );
//    m_internalSerializer = serializer;
//    m_read = m_internalSerializer->IsLoading();
//}
//
//bbool CSerializerSceneDep::OpenGroup( const char* tag, uSize index /* = 0 */ )
//{
//    return m_internalSerializer->OpenGroup( tag, index );
//}
//
//void CSerializerSceneDep::CloseGroup()
//{
//    m_internalSerializer->CloseGroup();
//}
//
//void CSerializerSceneDep::Serialize( const char* tag, unsigned long& ul )
//{
//    ITF_ASSERT_MSG( validatePathTag( tag ), "The tag %s should be a path", tag );
//    m_internalSerializer->Serialize( tag, ul );
//}
//
//void CSerializerSceneDep::Serialize( const char* tag, char& b )
//{
//    ITF_ASSERT_MSG( validatePathTag( tag ), "The tag %s should be a path", tag );
//    m_internalSerializer->Serialize( tag, b );
//}
//
//void CSerializerSceneDep::Serialize( const char* tag, unsigned char& b )
//{
//    ITF_ASSERT_MSG( validatePathTag( tag ), "The tag %s should be a path", tag );
//    m_internalSerializer->Serialize( tag, b );
//}
//
//void CSerializerSceneDep::Serialize( const char* tag, short& s )
//{
//    ITF_ASSERT_MSG( validatePathTag( tag ), "The tag %s should be a path", tag );
//    m_internalSerializer->Serialize( tag, s );
//}
//
//void CSerializerSceneDep::Serialize( const char* tag, int& i )
//{
//    ITF_ASSERT_MSG( validatePathTag( tag ), "The tag %s should be a path", tag );
//    m_internalSerializer->Serialize( tag, i );
//}
//
//void CSerializerSceneDep::Serialize( const char* tag, u32& ui )
//{
//    ITF_ASSERT_MSG( validatePathTag( tag ), "The tag %s should be a path", tag );
//    m_internalSerializer->Serialize( tag, ui );
//}
//
//void CSerializerSceneDep::Serialize( const char* tag, float& i )
//{
//    ITF_ASSERT_MSG( validatePathTag( tag ), "The tag %s should be a path", tag );
//    m_internalSerializer->Serialize( tag, i );
//}
//
//void CSerializerSceneDep::Serialize( const char* tag, double& i )
//{
//    ITF_ASSERT_MSG( validatePathTag( tag ), "The tag %s should be a path", tag );
//    m_internalSerializer->Serialize( tag, i );
//}
//
//void CSerializerSceneDep::Serialize( const char* tag, bool& b )
//{
//    ITF_ASSERT_MSG( validatePathTag( tag ), "The tag %s should be a path", tag );
//    m_internalSerializer->Serialize( tag, b );
//}
//
//void CSerializerSceneDep::Serialize( const char* tag, String& str )
//{
//    ITF_ASSERT_MSG( validatePathTag( tag ), "The tag %s should be a path", tag );
//    m_internalSerializer->Serialize( tag, str );
//    if ( str.strstr( '.', btrue ) != NULL )
//        m_paths.push_back( str );
//}
//
//void CSerializerSceneDep::Serialize( const char* tag, Vec2d& vec )
//{
//    ITF_ASSERT_MSG( validatePathTag( tag ), "The tag %s should be a path", tag );
//    m_internalSerializer->Serialize( tag, vec );
//}
//
//void CSerializerSceneDep::Serialize( const char* tag, Vec3d& vec )
//{
//    ITF_ASSERT_MSG( validatePathTag( tag ), "The tag %s should be a path", tag );
//    m_internalSerializer->Serialize( tag, vec );
//}
//
//void CSerializerSceneDep::Serialize( const char* tag, ObjectPath& _objPath )
//{
//    ITF_ASSERT_MSG( validatePathTag( tag ), "The tag %s should be a path", tag );
//    m_internalSerializer->Serialize( tag, _objPath );
//}
//
//void CSerializerSceneDep::Serialize( const char* tag, Color& _color )
//{
//    ITF_ASSERT_MSG( validatePathTag( tag ), "The tag %s should be a path", tag );
//    m_internalSerializer->Serialize( tag, _color );
//}
//
//void CSerializerSceneDep::Serialize( const char* tag, ObjectId& _id )
//{
//    ITF_ASSERT_MSG( validatePathTag( tag ), "The tag %s should be a path", tag );
//    m_internalSerializer->Serialize( tag, _id );
//}
//
//void CSerializerSceneDep::Serialize( const char* tag, String8& _id )
//{
//    ITF_ASSERT_MSG( validatePathTag( tag ), "The tag %s should be a path", tag );
//    m_internalSerializer->Serialize( tag, _id );
//}
//
//void CSerializerSceneDep::Serialize( const char* tag, ITF_STDSTRING& _name )
//{
//    ITF_ASSERT_MSG( validatePathTag( tag ), "The tag %s should be a path", tag );
//    m_internalSerializer->Serialize( tag, _name );
//}
//
//void CSerializerSceneDep::Serialize( const char* tag, Angle& _angle )
//{
//    ITF_ASSERT_MSG( validatePathTag( tag ), "The tag %s should be a path", tag );
//    m_internalSerializer->Serialize( tag, _angle );
//}
//
//void CSerializerSceneDep::Serialize( const char* tag, AngleAmount& _angle )
//{
//    ITF_ASSERT_MSG( validatePathTag( tag ), "The tag %s should be a path", tag );
//    m_internalSerializer->Serialize( tag, _angle );
//}
//
//void CSerializerSceneDep::Serialize( const char* tag, Path& _path )
//{
//    m_internalSerializer->Serialize( tag, _path );
//    m_paths.push_back( _path );
//}
//
//void CSerializerSceneDep::Serialize( const char* tag, StringID& _id )
//{
//    ITF_ASSERT_MSG( validatePathTag( tag ), "The tag %s should be a path", tag );
//    m_internalSerializer->Serialize( tag, _id );
//}
//
//void CSerializerSceneDep::Serialize( const char* tag, Volume& _volume )
//{
//    ITF_ASSERT_MSG( validatePathTag( tag ), "The tag %s should be a path", tag );
//    m_internalSerializer->Serialize( tag, _volume );
//}
//
//void CSerializerSceneDep::Serialize( const char* tag, LocalisationId& id )
//{
//    m_internalSerializer->Serialize( tag, id.value );
//    m_loc.push_back( id );
//}
//
//void CSerializerSceneDep::SerializeEnumBegin( const char* tag, int& selection )
//{
//    ITF_ASSERT_MSG( validatePathTag( tag ), "The tag %s should be a path", tag );
//    m_internalSerializer->SerializeEnumBegin( tag, selection );
//}
//
//void CSerializerSceneDep::SerializeEnumVar( int var_num, const char* var )
//{
//    m_internalSerializer->SerializeEnumVar( var_num, var );
//}
//
//void CSerializerSceneDep::SerializeEnumEnd()
//{
//    m_internalSerializer->SerializeEnumEnd();
//}
//
//bbool CSerializerSceneDep::ReadArrayCount( const char* tag, uSize& count )
//{
//    return m_internalSerializer->ReadArrayCount( tag, count );
//}
//
//bbool CSerializerSceneDep::WriteArrayCount( const char* tag, uSize count )
//{
//    return m_internalSerializer->WriteArrayCount( tag, count );
//}
//
//bool CSerializerSceneDep::validatePathTag( const char* tag ) const
//{
//    if ( m_pathTags == NULL )
//        return true;
//    for ( u32 i = 0; m_pathTags[i] != NULL; ++i )
//        if ( !strcmp( m_pathTags[i], tag ) )
//            return bfalse;
//    return btrue;
//}
//
//}
