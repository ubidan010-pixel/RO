#ifndef _ITF_ACCOUNT_H_
#define _ITF_ACCOUNT_H_

#include "core/UUID.h"
#include "core/String8.h"

namespace ITF
{
    struct NativeAccountId
    {
    };

    class Account
    {
    public:
        Account();
        virtual ~Account() 
		{
			if (m_nativePlatformId)
				delete m_nativePlatformId;
		}

        template<typename SpecializedNativeType, typename... Args>
        void setNativePlatformId(Args... args)
        {
            m_nativePlatformId = newAlloc(mId_Misc, SpecializedNativeType(args...));
        }

        NativeAccountId* getNativePlatformId() { return m_nativePlatformId; }
        NativeAccountId* getNativePlatformId() const { return m_nativePlatformId; }

        const String8& getName() const { return m_name; }
        void setName(const String8& _name) { m_name = _name; }

        const String8& getNameOnUplay() const { return m_nameOnUplay; }
        void setNameOnUplay(const String8& _nameOnUplay) { m_nameOnUplay = _nameOnUplay; }

        const UUID& getNetworkId() const { return m_networkId; }
        void setNetworkId(const UUID& _networkId) { m_networkId = _networkId; }

        const String8& getPlatformId() const { return m_platformId; }
        void setPlatformId(const String8& _platformId) { m_platformId = _platformId; }

        bool isLinkedToUplay() const { return m_isLinkedToUplay; } 
        void setLinkedToUplay(bool _isLinkedToUplay) { m_isLinkedToUplay = _isLinkedToUplay; }

        bool isLinkedToFacebook() const { return m_isLinkedToFacebook; }
        void setLinkedToFacebook(bool _isLinkedToFacebook) { m_isLinkedToFacebook = _isLinkedToFacebook; }

        bool isSignedOnPlatformNetwork() const { return m_isSignedOnPlatformNetwork; }
        void setSignedOnPlatformNetwork(bool _isSignedOnPlatformNetwork) { m_isSignedOnPlatformNetwork = _isSignedOnPlatformNetwork; }

        virtual void dumpToLog() const;

        struct OnlineAccountInfo
        {
            String8 m_nameOnPlatform;
            String8 m_idOnPlatform;
            u32     m_permissions;
        };

        enum EPermissions
        {
            EPermissions_None                       = 0,
            EPermissions_OnlineInteraction          = (1 << 0),
            EPermissions_UserCreatedContent         = (1 << 1),
            EPermissions_SocialNetworkSharing       = (1 << 2),     // Sharing activity on Facebook or Twitter
            EPermissions_MultimediaSharing          = (1 << 3),     // Sharing photos, videos or audio clips
            EPermissions_Shop                       = (1 << 4),
            EPermissions_UplayCreation              = (1 << 5),
            EPermissions_CrossPlatformGameplay      = (1 << 6),
            
            // Those 2 values must stay at the end of the list
            EPermissions_LastPlusOne,
            EPermissions_All                            = ((EPermissions_LastPlusOne - 1) << 1) - 1,
        };

        enum EPermissions_Status
        {
            EPermissions_StatusWaiting,
            EPermissions_StatusUnauthorized,
            EPermissions_StatusAuthorized
        };

        u32 getPermissions() const { return m_permissions; }
        String8 getPermissionsString() const;
        void setPermissions(u32 _permissions) { m_permissions = _permissions; }
		void addPermissions(u32 _permissions)
		{
			m_permissions |= _permissions;
		}
		void removePermissions(u32 _permissions)
		{
			m_permissions &= ~_permissions;
		}

        virtual EPermissions_Status checkPermissions(u32 _permissions) const
        {
			return ((m_permissions & _permissions) == _permissions) ? EPermissions_StatusAuthorized : EPermissions_StatusUnauthorized;
        }

        enum EAgeGroup
        {
            EAgeGroup_Child,
            EAgeGroup_Teenager,
            EAgeGroup_Adult,
        };

        EAgeGroup getAgeGroup() const { return m_ageGroup; }
        void setAgeGroup(EAgeGroup _ageGroup) { m_ageGroup = _ageGroup; }

        void setIsGuestAccount(bool flag) { m_isGuestAccount = flag; }
        bool isGuestAccount() const { return m_isGuestAccount; }

    protected:

        template<typename SpecializedNativeType = NativeAccountId>
        const SpecializedNativeType* getNativePlatformId() const
        {
			return static_cast<const SpecializedNativeType*>(m_nativePlatformId);
        }

        template<typename SpecializedNativeType = NativeAccountId>
        SpecializedNativeType* getNativePlatformId()
        {
			return static_cast<SpecializedNativeType*>(m_nativePlatformId);
        }

        NativeAccountId* m_nativePlatformId;
        String8     m_name;
        String8     m_nameOnUplay;
        UUID        m_networkId;
        String8     m_platformId;
        bool        m_isGuestAccount;
        bool        m_isLinkedToUplay;
        bool        m_isLinkedToFacebook;
        bool        m_isSignedOnPlatformNetwork;  // Signed on PSN / Xbox Live / ...
        u32         m_permissions;
        EAgeGroup   m_ageGroup;

        void addPermissionToString(String8& _string, EPermissions _permission, const char* _permissionName) const;

        friend class AccountAdapter;
    };


} // namespace ITF

#endif //_ITF_ACCOUNT_H_
