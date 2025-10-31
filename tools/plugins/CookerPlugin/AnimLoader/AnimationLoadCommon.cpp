#include "core/enginecommon.h"

#ifndef _ITF_ANIMATIONLOADCOMMON_H_
#include "tools/plugins/CookerPlugin/AnimLoader/AnimationLoadCommon.h"
#endif //_ITF_ANIMATIONLOADCOMMON_H_

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_


namespace ITF
{


void AnimationLoadCommon::xmlReadBoneDyn(PugiXMLWrap *_pXML, AnimBoneDyn *_pBoneDyn,PugiXMLWrap::ErrorBehavior errBehavior)
{
    _pBoneDyn->m_Length        = _pXML->readFloatAttribute("Lenght/value", 0);
    _pBoneDyn->m_Alpha         = _pXML->readFloatAttribute("Alpha/value", 1.f);

    _pBoneDyn->m_PosLocal.m_x  = _pXML->readFloatAttribute("PosLocal/x", 0);
    _pBoneDyn->m_PosLocal.m_y  = _pXML->readFloatAttribute("PosLocal/y", 0);
    _pBoneDyn->m_AngleLocal    = _pXML->readFloatAttribute("AngleLocal/value", 0) * MTH_DEGTORAD;

    _pBoneDyn->m_Zorder        = _pXML->readFloatAttribute("Zorder/value", 0);

    // Fix
    _pBoneDyn->m_Scale         = Vec2d(1.f, 1.f);

    // Generated
    _pBoneDyn->m_Pos           = Vec2d(0.f, 0.f);
    _pBoneDyn->m_Angle         = 0.f;
}


String AnimationLoadCommon::xmlReadBone(PugiXMLWrap *_pXML, AnimBone *_pBone, PtrToStrID *_ptrToStrID,  PugiXMLWrap::ErrorBehavior errBehavior)
{
    String UID;
    String parentUID;
    String name;
    UID.setText(_pXML->readAttribute("UID/value", "", errBehavior));
    parentUID.setText(_pXML->readAttribute("ParentUID/value", ""));
    if (!parentUID.isEmpty())
        (*_ptrToStrID)[&_pBone->m_BoneParent] = parentUID;

    _pBone->m_PatchFlags = 0;
    if (!strcmp(_pXML->readAttribute("ParentCut/value", ""), "true"))
        _pBone->m_PatchFlags |= BONE_PARENT_CUT;

    name.setText(_pXML->readAttribute("Name/value", ""));
    name.toLower();
    if (!name.isEmpty())
        _pBone->m_Name = name;
    else
        _pBone->m_Name.invalidate();

    if (!strcmp(_pXML->readAttribute("noBegin/value", "true"), "true"))
        _pBone->m_PatchFlags |= BONE_NO_BEGIN;
    if (!strcmp(_pXML->readAttribute("noEnd/value", "true"), "true"))
        _pBone->m_PatchFlags |= BONE_NO_END;

    if (_pXML->enterElement("RefsUID"))
    {
        ITF_VECTOR<String> ppPtr;
        if (_pXML->enterElement("Element"))
            do
            {
                _pBone->m_PatchPointsPtr.push_back(NULL);
                ppPtr.push_back(_pXML->readAttribute("value", "error", errBehavior));
            } while (_pXML->nextSameElement());

            for (u32 i=0; i<ppPtr.size(); i++)
            {
                (*_ptrToStrID)[&_pBone->m_PatchPointsPtr[i]] = ppPtr[i];
            }

            _pXML->exitElement();
    }
    return UID;
}

#if !defined(ITF_FINAL) && defined(ITF_DEBUG_STRINGID) && defined(ITF_WINDOWS)
///////////////////////////////////////////////////////////////////////////////////////////
pugi::xml_node AnimationLoadCommon::xmlWritePointer(pugi::xml_node _parentNode, const char *_name, void *_ptr)
{
    pugi::xml_node elem = _parentNode.append_child(_name);

    char buffer[256];
    sprintf(buffer, "%p", _ptr);
    elem.append_attribute("type").set_value("string");
    elem.append_attribute("value").set_value(buffer);
    return elem;
}
///////////////////////////////////////////////////////////////////////////////////////////
pugi::xml_node AnimationLoadCommon::xmlWriteString(pugi::xml_node _parentNode, const char *_name, const char *_string)
{
    pugi::xml_node elem = _parentNode.append_child(_name);

    elem.append_attribute("type").set_value("string");
    elem.append_attribute("value").set_value(_string);
    return elem;
}
///////////////////////////////////////////////////////////////////////////////////////////
pugi::xml_node AnimationLoadCommon::xmlWriteBool(pugi::xml_node _parentNode, const char *_name, bbool _value)
{
    pugi::xml_node elem = _parentNode.append_child(_name);

    elem.append_attribute("type").set_value("boolean");
    elem.append_attribute("value").set_value((_value?"true":"false"));
    return elem;
}
///////////////////////////////////////////////////////////////////////////////////////////
pugi::xml_node AnimationLoadCommon::xmlWriteFloat(pugi::xml_node _parentNode, const char *_name, f32 _value)
{
    pugi::xml_node elem = _parentNode.append_child(_name);

    elem.append_attribute("type").set_value("number");
    if (_value==(f32)(int)_value)
    {
        elem.append_attribute("value").set_value((int)_value);
    }
    else
    {
        elem.append_attribute("value").set_value(_value);
    }
    return elem;
}

///////////////////////////////////////////////////////////////////////////////////////////
pugi::xml_node AnimationLoadCommon::xmlWriteVector(pugi::xml_node _parentNode, const char *_name, const Vec2d &_vec)
{
    pugi::xml_node elem = _parentNode.append_child(_name);

    elem.append_attribute("type").set_value("vector");
    elem.append_attribute("x").set_value( _vec.m_x);
    elem.append_attribute("y").set_value( _vec.m_y);
    elem.append_attribute("z").set_value(0);
    return elem;
}
///////////////////////////////////////////////////////////////////////////////////////////
void AnimationLoadCommon::xmlWriteBone(pugi::xml_node _parentNode, AnimBone *_bone, AnimSkeleton *_skeleton)
{
    xmlWriteString(_parentNode, "Name", _bone->m_Name.getDebugString());

    xmlWritePointer(_parentNode, "UID", _bone);

    xmlWriteBool(_parentNode, "noBegin", (_bone->m_PatchFlags & BONE_NO_BEGIN));

    xmlWriteBool(_parentNode, "noEnd", (_bone->m_PatchFlags & BONE_NO_END));

    xmlWriteBool(_parentNode, "Mirror", bfalse); ///////////////////////////////

    xmlWriteFloat(_parentNode, "ScaleX", 1.f);
    xmlWriteFloat(_parentNode, "ScaleY", 1.f);

    if (_bone->m_PatchFlags & BONE_PARENT_CUT)
        xmlWriteBool(_parentNode, "ParentCut", btrue);

    if (_bone->m_BoneParent)
        xmlWritePointer(_parentNode, "ParentUID", _bone->m_BoneParent);

    pugi::xml_node refsUIDNode = _parentNode.append_child("RefsUID");
    {
        for (u32 index=0; index<_bone->m_PatchPointsPtr.size(); index++)
        {
            AnimPatchPoint *point = _bone->m_PatchPointsPtr[index];
            pugi::xml_node element = xmlWritePointer(refsUIDNode, "Element", point);
            element.append_attribute("index").set_value(index+1);
        }        

    }

}

///////////////////////////////////////////////////////////////////////////////////////////
void AnimationLoadCommon::xmlWriteBoneDyn(pugi::xml_node _parentNode, AnimBoneDyn *_bone, AnimTemplate *_template)
{
    f32 radToDeg = 180.f/MTH_PI;

    xmlWriteVector(_parentNode, "Pos", _bone->m_Pos);

    xmlWriteVector(_parentNode, "PosEnd", _bone->m_PosEnd);

    xmlWriteFloat(_parentNode, "Angle", _bone->m_Angle*radToDeg);

    xmlWriteFloat(_parentNode, "Lenght", _bone->m_Length);

    xmlWriteFloat(_parentNode, "Alpha", _bone->m_Alpha);

    xmlWriteVector(_parentNode, "PosLocal", _bone->m_PosLocal);

    xmlWriteFloat(_parentNode, "AngleLocal", _bone->m_AngleLocal*radToDeg);

    xmlWriteFloat(_parentNode, "Zorder", _bone->m_Zorder);

    xmlWritePointer(_parentNode, "TemplateUID", _template);
}

#endif

};

