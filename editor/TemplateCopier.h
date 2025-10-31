#ifndef _ITF_TEMPLATECOPIER_H_
#define _ITF_TEMPLATECOPIER_H_

#ifndef _ITF_STRING_H_
#include "core/itfstring.h"
#endif //_ITF_STRING_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///Template copier
    class TemplateCopier
    {
    public:
        ///////////////////////////////////////////////////////////////////////////////////////////
        static bbool copyTemplateAndDependenciesWithGUI();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// copyTemplateAndDependencies
        /// @param _templateFile the file to copy (absolute path)
        /// @param _destinationDirectory
        /// @param _newTemplateName
        /// @param _copiedFilename renaming of _templateFile
        static bbool copyTemplateAndDependencies(const String &_templateFile,
            const String &_destinationDirectory,
            const String &_newTemplateName,
            String &_copiedFilename);

    };
}


#endif
