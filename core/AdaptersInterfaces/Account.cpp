#include "precompiled_core.h"

#include "core/AdaptersInterfaces/Account.h"
#include "core/error/ErrorHandler.h"

namespace ITF
{
    namespace
    {
        struct PermissionDesc
        {
            Account::EPermissions   m_permission;
            const char*             m_name;
			PermissionDesc(Account::EPermissions   permission,
				const char*             name)
			{
				m_permission = permission;
				m_name = name;
			}
			PermissionDesc()
			{
				m_permission = Account::EPermissions::EPermissions_None;
				m_name = NULL;
			}
        };

		PermissionDesc gs_permissionDescs[7];
    }

    Account::Account()
        : m_nativePlatformId(nullptr)
        , m_name()
        , m_networkId()
        , m_isGuestAccount(false)
        , m_isLinkedToUplay(false)
        , m_isLinkedToFacebook(false)
        , m_isSignedOnPlatformNetwork(false)
        , m_permissions(EPermissions_None)
        , m_ageGroup(EAgeGroup_Child)
    {
		//populating the gs_permissionDescs array
		#define PERMISSION_DESC(X, PERM_NAME) gs_permissionDescs[X] = PermissionDesc( Account::EPermissions_##PERM_NAME, #PERM_NAME);
		PERMISSION_DESC(0,OnlineInteraction)
		PERMISSION_DESC(1,UserCreatedContent)
		PERMISSION_DESC(2,SocialNetworkSharing)
		PERMISSION_DESC(3,MultimediaSharing)
		PERMISSION_DESC(4,Shop)
		PERMISSION_DESC(5,UplayCreation)
		PERMISSION_DESC(6,CrossPlatformGameplay)
		#undef PERMISSION_DESC
    }


    void Account::dumpToLog() const
    {
        LOG("\t* Cross-platform properties:"
            "\n\t\t- Name: %s"
            "\n\t\t- Network ID: %s"
            "\n\t\t- Is linked to Uplay? %s"
            "\n\t\t- Is linked to Facebook? %s"
            "\n\t\t- Is signed on platform network? %s"
            "\n\t\t- Permissions: %s"
            "\n\t\t- Age group: %s"
            , m_name.cStr()
            , m_networkId.toString().cStr()
            , (m_isLinkedToUplay ? "yes" : "no")
            , (m_isLinkedToFacebook ? "yes" : "no")
            , (m_isSignedOnPlatformNetwork ? "yes" : "no")
            , getPermissionsString().cStr()
            , (m_ageGroup == EAgeGroup_Child ? "Child" : (m_ageGroup == EAgeGroup_Teenager ? "Teenager" : "Adult"))
            );
    }

    String8 Account::getPermissionsString() const
    {
        String8 result;

        if (m_permissions != EPermissions_None)
        {
            for (size_t descInd = 0; descInd < ITF_ARRAY_SIZE(gs_permissionDescs); descInd++)
            {
                const PermissionDesc& permissionDesc = gs_permissionDescs[descInd];
                addPermissionToString(result, permissionDesc.m_permission, permissionDesc.m_name);
            }

            ITF_ASSERT(!result.isEmpty());
            result.truncate(result.getLen() - 2);  // Remove the extra ", " at the end
        }
        else
        {
            result = "None";
        }

        return result;
    }


    void Account::addPermissionToString(String8& _string, EPermissions _permission, const char* _permissionName) const
    {
        switch (checkPermissions(_permission))
        {
            case EPermissions_StatusUnauthorized:
                // Nothing to do, just return
                return;

            case EPermissions_StatusAuthorized:
                _string += _permissionName;
                _string += ", ";
                break;

            case EPermissions_StatusWaiting:
                _string += "(";
                _string += _permissionName;
                _string += "?), ";
                break;
        }
    }

} // namespace ITF
