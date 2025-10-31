#include "core/enginecommon.h"

#ifndef _ITF_ANIMATIONLOADSKELETON_H_
#include "tools/plugins/CookerPlugin/AnimLoader/AnimationLoadSkeleton.h"
#endif //_ITF_ANIMATIONLOADSKELETON_H_

#ifndef _ITF_ANIMATIONLOADPOLYLINE_H_
#include "tools/plugins/CookerPlugin/AnimLoader/AnimationLoadPolyline.h"
#endif //_ITF_ANIMATIONLOADPOLYLINE_H_

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif // _ITF_FILESERVER_H_

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

namespace ITF
{

    static const String POLYLINE_EXT(".pol");

    AnimSkeleton * AnimationLoadSkeleton::readAnimSkeleton  (  const Path& _XMLFile_Path,const String& _szPlatform, bbool _raiseError, bbool _bSaveBin)
    {
        PugiXMLWrap::ErrorBehavior errBehavior = _raiseError ? PugiXMLWrap::RaiseError : PugiXMLWrap::DoNothing;
        PathString_t _szXMLFile;
        
        _XMLFile_Path.getString(_szXMLFile);

        PugiXMLWrap     XML;
        PtrToStrID      ptrToStrID;

        String          cookFile    = FILESERVER->getCookedNamePlatform(_szXMLFile,_szPlatform);
        AnimSkeleton *  animSkeleton=  newAlloc(mId_Animation,AnimSkeleton());
        animSkeleton->m_flushTime   = FILEMANAGER->getLastTimeWriteAccess(_szXMLFile);

        if (!XML.openFile(_szXMLFile,bfalse, errBehavior))
        {
            SF_DEL(animSkeleton);
            return NULL;
        }

        if (XML.enterElement("BonesListT"))
        {
            int pos = 0;
			ITF_ASSERT(animSkeleton->m_BonesCurFrame.empty());
			u32 boneNb = XML.getNumChildren("Element");
			animSkeleton->m_BonesCurFrame.resize(boneNb);

            if (XML.enterElement("Element"))
			{
                do
                {
					AnimBone *bone = &animSkeleton->m_BonesCurFrame[pos];
                    String      boneUID = AnimationLoadCommon::xmlReadBone(&XML,bone , &ptrToStrID, errBehavior);

                    if (bone->m_Name.isValid())
                        animSkeleton->m_NameToBone.setID(bone->m_Name, pos);
                    animSkeleton->m_UIDToBone.setID(boneUID, pos++);

                    AnimBoneDyn boneDyn;
                    AnimationLoadCommon::xmlReadBoneDyn(&XML, &boneDyn,errBehavior);
                    // set dynamic default value
#ifndef NO_RUNTIME_LOCK
                    boneDyn.m_LockedIndex = U32_INVALID;
#endif
                    animSkeleton->m_BonesTPauseDyn.push_back(boneDyn);
                } while (XML.nextSameElement());
                XML.exitElement();
			}
			ITF_ASSERT(static_cast<int>(boneNb) == pos);
        }

        // resolve ptr
        for (VectorAnim<AnimBone>::iterator iter = animSkeleton->m_BonesCurFrame.begin();
            iter != animSkeleton->m_BonesCurFrame.end(); ++iter)
        {
            PtrToStrID::iterator ptrIter = ptrToStrID.find(&(*iter).m_BoneParent);
            if (ptrIter != ptrToStrID.end())
            {
                NamesMap::Iterator parentIter(animSkeleton->m_UIDToBone);
                parentIter.findKey((*ptrIter).second);
                ITF_ASSERT(!parentIter.isEnd());
                if (!parentIter.isEnd())
                {
                    (*iter).m_BoneParent = &animSkeleton->m_BonesCurFrame[parentIter.second()];
                }
            }
        }

        // read also polyline
        String polylineStr = FilePath::getDirectory(_szXMLFile) + FilePath::getFilenameWithoutExtension(_szXMLFile) + POLYLINE_EXT;
        animSkeleton->m_PolylineBank = AnimationLoadPolyline::readAnimPolylineBank(polylineStr, _szPlatform, _raiseError, bfalse);

        if (_bSaveBin)//Used in cooker mode
        {
            ITF_VERIFY_MSG(animSkeleton->saveBinary(cookFile), "Failed to save cooked version");
        }

        return animSkeleton;
    }

};
