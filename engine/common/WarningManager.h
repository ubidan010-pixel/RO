#ifndef _ITF_WARNINGMANAGER_H_
#define _ITF_WARNINGMANAGER_H_

#ifndef ITF_FINAL

#include "engine/common/WarningCategories.h"

#ifndef _ITF_STRING_H_
#include "core/itfstring.h"
#endif //_ITF_STRING_H_

#ifndef _ITF_SAFEARRAY_H_
#include "core/containter/SafeArray.h"
#endif //_ITF_SAFEARRAY_H_

namespace ITF
{
    class BaseObject;
    class String;
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    class WarningManager
    {
    public:
        
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// initialization. Call before any other methode
        static void initialize();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// termination
        static void terminate();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// clear the buffer
        static void clear();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// returns true if there is a warning for the given object
        /// @param _owner
        static bool hasWarningForObject(const BaseObject *_owner);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// remove all warnings for given object
        static void clearWarningsForObject(const BaseObject *_owner);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// returns true if buffer is empty
        static bool isEmpty();
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// add a warning. There is a limited size in the buffer, so it may not be added
        /// @param _category The category. 0 is reserved for unknown category.
        /// @param _owner The BaseObject owner. May be NULL.
        /// @param _param The parameter name. May be NULL
        /// @param _messageFormat. The warning message
        static void addWarning(WarningCategoryIndex _category, const BaseObject *_owner, const char *_param, const char *_messageFormat,...);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// filter display for given objects. There is a limited size, which is the same as the max number of warnings
        /// @param _owner Table of objects to filter
        /// @param _itemCount Count of items in the table
        static void filterDisplayForObjects(BaseObject *_owner[], unsigned int _itemCount);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// display : called at each display loop
        static void display();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// returnFalse : utility function for returning false (and thus avoid warnings)
        static bool returnFalse() {return false;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// return list of warnings
        static void getWarningsList(ITF_VECTOR<String>& _warnings);

    private:
    };
}
#endif

#endif
