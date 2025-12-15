#ifndef __WWISE_AKGAMEOBJECT_H__
#define __WWISE_AKGAMEOBJECT_H__
#ifdef ITF_SUPPORT_WWISE
#include "AK/SoundEngine/Common/AkTypes.h"
#include "core/math/vec3d.h"
#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter_Enums.h"
#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter_Types.h"

#ifndef _ITF_BASEOBJECT_H_
#include "core/baseObject.h"
#endif //_ITF_BASEOBJECT_H_

namespace	ITF
{
	class		ObjectRef;

	namespace	Wwise
    {
	    class	AkGameObject;

	    //////////////////////////////////////////////////////////////////////////
	    //
	    class	AkGameObjectFactory
	    {


	    public:
			    static void							s_createSingleton();
			    static void							s_destroySingleton();
			    static inline AkGameObjectFactory	*s_getSingleton()	{ return ms_singleton; }

			    AkGameObjectID						create(const ObjectRef &_objectRef);
			    void								destroy(AkGameObjectID _akGameObjectID);
                ObjectRef                           createAkActor(const char* _name = nullptr);

                bool                                isRegistered(const AkGameObjectID _akGameObjectID);
			    AkGameObject						*getAkGameObject(const ObjectRef &_objectRef);
			    AkGameObject						*getAkGameObject(AkGameObjectID _akGameObjectID);

	    private:
			    static AkGameObjectFactory						*ms_singleton;
			    typedef ITF_MAP<AkGameObjectID, AkGameObject *> AkGameObjectMap;
			    AkGameObjectMap									m_akGameObjectMap;
	    };

	    //////////////////////////////////////////////////////////////////////////
	    //
	    class	AkGameObject
	    {
			    friend class	AkGameObjectFactory;

	    public:
                static const AkGameObjectID GLOBAL_GAME_OBJECT_ID = (AkGameObjectID)(ListenerID::eListenerCount);
                static const char			*GLOBAL_GAME_OBJECT_NAME ;

                static AkGameObjectID   s_getAkGameObjectID(const ObjectRef &_objectRef);
                static ObjectRef		s_getObjectRef(AkGameObjectID _akGameObjectID);

			    void	                updatePosition(bool *_isAlive, bool *_positionChanged);
			    inline const Vec3d&     getPosition() const {return m_position;}
			    const char	*           getName() const;
                void                    setPosition(const ITF_VECTOR<Vec3d>&_positions, AudioMultiPositionMode _mode);


	    protected:
			    explicit	AkGameObject(const ObjectRef &_objectRef);	// register
			    ~			AkGameObject(); // unregister


	    private:
			    Vec3d		m_position;
			    f32			m_angle;
			    ObjectRef	m_objectRef;
                bool        m_isMultiPosition;

			    inline bool isEqualWithEpsilon(f32 x, f32 y, f32 epsilon = MTH_EPSILON) const {return fabsf(x - y) <= epsilon;}
                inline bool isEqualWithEpsilon(const Vec3d &a, const Vec3d &b, f32 epsilon = MTH_EPSILON) const { return isEqualWithEpsilon(a.getX(), b.getX(), epsilon) && isEqualWithEpsilon(a.getY(), b.getY(), epsilon) && isEqualWithEpsilon(a.getZ(), b.getZ(), epsilon); }
	    };

    }

    class AkActor : public BaseObject
    {
    public:
        AkActor(const char* _name);
        virtual ~AkActor() {}
#if !defined( ITF_FINAL) || defined( ITF_WWISE_PROFILE)
        ITF_INLINE const String8& getUserFriendly() const { return m_userFriendly; }
#endif


    private:
#if !defined( ITF_FINAL) || defined( ITF_WWISE_PROFILE)
        ITF_INLINE void             setUserFriendly(const String8& _name) { m_userFriendly = _name; }

        String8                 m_userFriendly;
#endif
    };
}
#endif // #ifdef ITF_SUPPORT_WWISE
#endif // __WWISE_AKGAMEOBJECT_H__
