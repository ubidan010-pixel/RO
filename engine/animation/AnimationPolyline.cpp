#include "precompiled_engine.h"

#ifndef _ITF_ANIMATIONPOLYLINE_H_
#include    "engine/animation/AnimationPolyline.h"
#endif // _ITF_ANIMATIONPOLYLINE_H_

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_ARCHIVELINKER_H_
#include "core/ArchiveLinker.h"
#endif //_ITF_ARCHIVELINKER_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif  //_ITF_VERSIONING_H_

namespace ITF
{

    Vec2d AnimPolylinePoint::ComputeGlobalPosInternally(AnimBoneDyn *B, const AnimGlobalData & _globalData)
    {
        f32     fMirror = _globalData.m_mirror ? -1.f : 1.f;

        Vec2d   VDir, Vect00, Vect90, Pos1, Pos2, vtmp;
        f32     fPMirror;
        f32     VDirLen;
        f32     d,d2;

        Pos1            = B->m_Pos;
        Pos2            = B->m_PosEnd;
        if (!_globalData.m_isRatioOne)
        {
            vtmp = Pos1 - _globalData.m_origin;
            d = vtmp.dot(_globalData.m_xAxis);
            d2 = vtmp.dot(_globalData.m_yAxis)*_globalData.m_ratio;
            Vec2d::ScaleAdd(&Pos1, &_globalData.m_xAxis, d, &_globalData.m_origin);
            Vec2d::ScaleAdd(&Pos1, &_globalData.m_yAxis, d2, &Pos1);

            vtmp = Pos2 - _globalData.m_origin;
            d = vtmp.dot(_globalData.m_xAxis);
            d2 = vtmp.dot(_globalData.m_yAxis)*_globalData.m_ratio;
            Vec2d::ScaleAdd(&Pos2, &_globalData.m_xAxis, d, &_globalData.m_origin);
            Vec2d::ScaleAdd(&Pos2, &_globalData.m_yAxis, d2, &Pos2);
        }

        VDir      		= (B->m_PosEnd - B->m_Pos);
        Vect00	        = VDir;
        VDirLen         = VDir.norm();
        Vect00          *= f32_Inv(VDirLen);
        fPMirror        = B->m_Scale.m_x * B->m_Scale.m_y >= 0.f ? 1.f : -1.f;
       
        Vec2d::SwapXmY(&Vect90, &Vect00);
        Vect90 *= fMirror * fPMirror;

        Vec2d res;
        //res = B->m_Pos +  VDir * m_PosLocal.m_x  + Vect90 * (m_PosLocal.m_y * f32_Abs(B->m_Scale.m_y * (VDirLen / B->GetScaledLength())));
        Vec2d::MulXAdd(&res, &VDir, &m_PosLocal, &B->m_Pos);
        Vec2d::ScaleAdd(&res, &Vect90, (m_PosLocal.m_y * f32_Abs(B->m_Scale.m_y * (VDirLen / B->GetScaledLength()))), &res);
        if (!_globalData.m_isRatioOne)
        {
            vtmp = res - _globalData.m_origin;
            d = vtmp.dot(_globalData.m_xAxis);
            d2 = vtmp.dot(_globalData.m_yAxis)*_globalData.m_ratio;
            Vec2d::ScaleAdd(&res, &_globalData.m_xAxis, d, &_globalData.m_origin);
            Vec2d::ScaleAdd(&res, &_globalData.m_yAxis, d2, &res);
        }
        return res;
    }


    Vec2d AnimPolylinePoint::ComputeGlobalPos(SafeArray<AnimBoneDyn> *_dyn, const AnimGlobalData & _globalData)
    {
        AnimBoneDyn *B      = &(*_dyn)[m_BonePtr->getIndex()];
        return ComputeGlobalPosInternally(B, _globalData);
    }

    Vec2d AnimPolylinePoint::ComputeRelativePos(Vec2d _scale, const AnimGlobalData & _globalData)
    {
        AnimBoneDyn boneDyn;
        boneDyn.m_Pos   = Vec2d::Zero;
        boneDyn.m_PosEnd= Vec2d::Right;
        boneDyn.m_Scale = _scale;
        boneDyn.m_Length= 1.f;
        
        return ComputeGlobalPosInternally(&boneDyn, _globalData);
    }


    void AnimPolylinePoint::serialize(ArchiveMemory& _pArchive)
    {
        // Serialize Polyline AFTER bone serialization !

        m_PosLocal.serialize(_pArchive);
        m_BoneUID.serialize(_pArchive);
    }

    AnimPolylineBank::AnimPolylineBank()
    {
    }

    void AnimPolyline::ComputeGlobalPos(SafeArray<AnimBoneDyn> *_dyn, Vec2d *_posList, const AnimGlobalData & _globalData)
    {
        ITF_ASSERT_MSG(_posList, "Position list is NULL!");
        VectorAnim<AnimPolylinePoint>::iterator ppIter = m_PolylinePointList.begin();
        for (u32 i=0; ppIter != m_PolylinePointList.end(); ++ppIter, ++i)
        {
            _posList[i] = (*ppIter).ComputeGlobalPos(_dyn, _globalData);
        }
    }

    void AnimPolyline::ComputeRelativePos(Vec2d* _posList, Vec2d _scale, const AnimGlobalData & _globalData)
    {
        ITF_ASSERT_MSG(_posList, "Position list is NULL!");
        VectorAnim<AnimPolylinePoint>::iterator ppIter = m_PolylinePointList.begin();
        for (u32 i=0; ppIter != m_PolylinePointList.end(); ++ppIter, ++i)
        {
            _posList[i] = (*ppIter).ComputeRelativePos(_scale, _globalData);
        }
    }

    bbool AnimPolyline::resolvePtr(AnimSkeleton *_skeleton)
    {
        bbool result = btrue;
        VectorAnim<AnimPolylinePoint>::iterator ppIter = m_PolylinePointList.begin();
        for (; ppIter != m_PolylinePointList.end(); ++ppIter)
        {
            AnimPolylinePoint * polyP = &(*ppIter);

            NamesMap::Iterator nameIter(_skeleton->m_UIDToBone);
            nameIter.findKey(polyP->m_BoneUID);

            if (nameIter.isValid() && nameIter.second() < (i32)_skeleton->m_BonesCurFrame.size())
            {
                polyP->m_BonePtr = &_skeleton->m_BonesCurFrame[nameIter.second()];
            } else
            {
                result = bfalse;
                if (!_skeleton->m_BonesOrdered.size())
                    _skeleton->ComputeBonesOrder();
                ITF_ASSERT(_skeleton->m_BonesOrdered.size());
                if (_skeleton->m_BonesOrdered.size())
                {
                    polyP->m_BonePtr = _skeleton->m_BonesOrdered[0];
                } else
                {
                    polyP->m_BonePtr = NULL;
                }
            }
        }
        return result;
    }

    void AnimPolyline::serialize(ArchiveMemory& _pArchive)
    {
        m_PolylinePointList.serialize(_pArchive);
        m_NameToPolylinePoint.serialize(_pArchive);
        m_Name.serialize(_pArchive);
        _pArchive.serialize(m_isLooping);
        _pArchive.serialize((u32 &)m_type);
    }


    bbool AnimPolylineBank::resolvePtr(AnimSkeleton *_skeleton)
    {
        bbool result = btrue;
        VectorAnim<AnimPolyline>::iterator polyIter = m_PolylineList.begin();
        for (; polyIter != m_PolylineList.end(); ++polyIter)
        {
            result = (*polyIter).resolvePtr(_skeleton) && result;
        }
        return result;
    }

    void AnimPolylineBank::serialize(ArchiveMemory& _pArchive, bbool )
    {
        m_PolylineList.serialize(_pArchive);
        m_NameToPolyline.serialize(_pArchive);
    }

};