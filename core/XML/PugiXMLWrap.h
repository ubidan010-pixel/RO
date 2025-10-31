#ifndef _ITF_PUGIXMLWRAP_H_
#define _ITF_PUGIXMLWRAP_H_

#ifdef ITF_SUPPORT_XML

#ifndef HEADER_PUGIXML_HPP
#include "pugixml/src/pugixml.hpp"
#endif //HEADER_PUGIXML_HPP

class File;

#ifndef _ITF_VEC2D_H_
#include "core/math/vec2d.h"
#endif //_ITF_VEC2D_H_

#ifndef _ITF_SAFEARRAY_H_
#include "core/container/SafeArray.h"
#endif //_ITF_SAFEARRAY_H_


namespace ITF
{
class PugiXMLWrap
{

public:
    /// enables the user to chose what TinyXMLWrapDeprecate should do when an error is met
    enum ErrorBehavior
    {
        DoNothing,      /// ignore the error
        LogWarning,     /// log a warning
        RaiseError      /// raise an error (assertion)
    };

    PugiXMLWrap                 (                               );
    ~PugiXMLWrap                (                               );
    
    /// @param _documentContent the XML content
    /// @param _err             what to do in case of error (@see ErrorBehavior)
    /// @return                 btrue if succeeds
    /// @param _enterRoot   tell if "root" element should be automatically entered
    bbool               openString                  (   const String& _documentContent, ErrorBehavior _err = DoNothing , bbool _enterRoot = btrue );
    bbool               openString                  (const char* _documentContent, ErrorBehavior _err, bbool _enterRoot);


    /// @param _fileName    the XML file to parse
    /// @param _bUseBinary  true to generate a binary xml
    /// @param _err         what to do in case of error (@see ErrorBehavior)
    /// @param _enterRoot   tell if "root" element should be automatically entered
    /// @return             btrue if succeeds
    bbool               openFile                    (  const String&  _szXMLFile,bbool _bUseBinary , ErrorBehavior _err = DoNothing, bbool _enterRoot = btrue   );

    /// @param _err         what to do in case of error (@see ErrorBehavior)
    /// @return             btrue if succeeds
    bbool               enterFirstElement           (   ErrorBehavior _err = DoNothing   );

    /// @param _elemName    the name of the XML tag to enter
    /// @param _err         what to do in case of error (@see ErrorBehavior)
    /// @return             btrue if succeeds
    bbool               enterElement                (   const char* _elemName, ErrorBehavior _err = DoNothing   );
    bbool               enterElement                (   const char* _elemName, size_t _index, ErrorBehavior _err = DoNothing   );

    /// exits to current element to get back to its father
    void                exitElement                 (                                                           );

    /// Finds the next similar element in the current hierarchy (finds next brother)
    /// @param _exitAtLast when set to btrue, automatically exits the current element (@see exitElement) if no brother is found
    /// @return             btrue if succeeds
    bbool               nextSameElement             (   bbool _exitAtLast = btrue                              );

    /// Finds the next element in the current hierarchy (finds next brother)
    /// @param _exitAtLast when set to btrue, automatically exits the current element (@see exitElement) if no brother is found
    /// @return             btrue if succeeds
    bbool               nextElement                 (   bbool _exitAtLast = btrue                              );


    /// @param _attrName    the name of the attribute to read
    /// @param _default     the returned default value if _attrName is not found
    /// @param _err         what to do in case of error (@see ErrorBehavior)
    /// @return             the attribute's value, or _default if the attribute was not found
    const char*         readAttribute               (   const char* _attrName, const char* _default = NULL, ErrorBehavior _err = DoNothing   );

    /// @param _attrName    the name of the attribute to read
    /// @param _default     the returned default value if _attrName is not found
    /// @param _err         what to do in case of error (@see ErrorBehavior)
    /// @return             the attribute's value, or _default if the attribute was not found
    int                 readIntAttribute            (   const char* _attrName, int _default = 0, ErrorBehavior _err = DoNothing   );

    /// @param _attrName    the name of the attribute to read
    /// @param _default     the returned default value if _attrName is not found
    /// @param _err         what to do in case of error (@see ErrorBehavior)
    /// @return             the attribute's value, or _default if the attribute was not found
    bbool               readBoolAttribute           (   const char* _attrName, bbool _default = false, ErrorBehavior _err = DoNothing   );

    /// @param _attrName    the name of the attribute to read
    /// @param _default     the returned default value if _attrName is not found
    /// @param _err         what to do in case of error (@see ErrorBehavior)
    /// @return             the attribute's value, or _default if the attribute was not found
    float               readFloatAttribute          (   const char* _attrName, float _default = 0.0f, ErrorBehavior _err = DoNothing   );

    /// @param _attrName    the name of the attribute to read
    /// @param _default     the returned default value if _attrName is not found
    /// @param _err         what to do in case of error (@see ErrorBehavior)
    /// @return             the attribute's value, or _default if the attribute was not found
    Vec2d               readVec2dAttribute          (   const char* _attrName, const Vec2d& _default = Vec2d::Zero, ErrorBehavior _err = DoNothing   );

    /// @return             the name of the current element
    const char *        readElementName             (                               );

    /// @return             the text of the current element
    const char *        readElementValue             (                               );
    
    /// used to display errors
    /// @return the concatenation of the current label and all of the node's parents labels
    char*               getFullName                 (                               );

	u32					getNumChildren(const char* _elemName);
	u32					getNumChildren();

    pugi::xml_node      getCurrentNode() { return m_currentNode; }
    static void         setModeSilent(bbool _bSilent) {m_bSilent = _bSilent;}

    /*debug*/ bbool     m_logEnable;
private:
    static bbool        m_bSilent;
    pugi::xml_document  m_doc;
    pugi::xml_node      m_currentNode;

    /*debug*/ String    m_szXMLFileDebug;
};


} // namespace ITF
#endif //ITF_SUPPORT_XML
#endif // _ITF_PUGIXMLWRAP_H_
