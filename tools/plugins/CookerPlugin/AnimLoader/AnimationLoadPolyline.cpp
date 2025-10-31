#include "core/enginecommon.h"

#ifndef _ITF_ANIMATIONLOADPOLYLINE_H_
#include "tools/plugins/CookerPlugin/AnimLoader/AnimationLoadPolyline.h"
#endif //_ITF_ANIMATIONLOADPOLYLINE_H_

#ifndef _ITF_ANIMATIONPOLYLINE_H_
#include    "engine/animation/AnimationPolyline.h"
#endif // _ITF_ANIMATIONPOLYLINE_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif // _ITF_FILESERVER_H_

namespace ITF
{
    AnimPolylineBank * AnimationLoadPolyline::readAnimPolylineBank(  const Path& _XMLFile_Path,const String& _szPlatform, bbool _raiseError, bbool _bSaveBin)
    {
        PugiXMLWrap::ErrorBehavior errBehavior = _raiseError ? PugiXMLWrap::RaiseError : PugiXMLWrap::DoNothing;
        PathString_t        _szXMLFile;
        
        _XMLFile_Path.getString(_szXMLFile);

        PugiXMLWrap         XML;

        String              cookFile         = FILESERVER->getCookedNamePlatform(_szXMLFile,_szPlatform);
        AnimPolylineBank *  animPolylineBank =  newAlloc(mId_Animation,AnimPolylineBank());

        if (!XML.openFile(_szXMLFile,bfalse, errBehavior))
        {
            SF_DEL(animPolylineBank);
            return NULL;
        }

        if (XML.enterFirstElement())
        do 
        {
            String Id = XML.readElementName();

            AnimPolyline polyline;
            u32          polylineIndex = animPolylineBank->m_PolylineList.size();
            animPolylineBank->m_PolylineList.push_back(polyline);

            animPolylineBank->m_NameToPolyline.setID(Id, polylineIndex);
            AnimPolyline *polylinePtr = &animPolylineBank->m_PolylineList[polylineIndex];
            polylinePtr->m_Name = Id;
            xmlReadAnimPolyline(&XML, polylinePtr, errBehavior);
        } while (XML.nextElement());

        if (_bSaveBin)//Used in cooker mode
        {
            ITF_VERIFY_MSG(animPolylineBank->saveBinary(cookFile), "Failed to save cooked version");
        }

        return animPolylineBank;
    }

    bbool  AnimationLoadPolyline::xmlReadAnimPolyline(PugiXMLWrap * _XML, AnimPolyline* _polyline, PugiXMLWrap::ErrorBehavior errBehavior)
    {
        if (_XML->enterElement("PointsList"))
        {
            if (_XML->enterElement("Element"))
                do
                {
                    AnimPolylinePoint p;
                    p.m_PosLocal.m_x       = _XML->readFloatAttribute("PosLocal/x", 0, errBehavior);
                    p.m_PosLocal.m_y       = _XML->readFloatAttribute("PosLocal/y", 0, errBehavior);
                    p.m_BoneUID            = _XML->readAttribute("BoneUID/value", "", errBehavior);
                    const char * polyName  = _XML->readAttribute("Name/value", "", errBehavior);

                    _polyline->m_NameToPolylinePoint.setID(polyName, _polyline->m_PolylinePointList.size());
                    _polyline->m_PolylinePointList.push_back(p);
                } while (_XML->nextSameElement());
                _XML->exitElement();
        }

        _polyline->m_isLooping = !strcmp(_XML->readAttribute("closed/value", ""), "true");
        _polyline->m_type = (AnimPolyline::PolylineType)_XML->readIntAttribute("type/value", 1);

        return _polyline->m_PolylinePointList.size();
    }


};
