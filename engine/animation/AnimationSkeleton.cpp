#include "precompiled_engine.h"

#include <algorithm>

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_ANIMATIONPOLYLINE_H_
#include    "engine/animation/AnimationPolyline.h"
#endif // _ITF_ANIMATIONPOLYLINE_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif  //_ITF_VERSIONING_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifdef ITF_IPAD
#include <algorithm>
#endif

namespace ITF
{
    cookFileCallback AnimSkeleton::m_funcCookerSkeleton = NULL;

    ITF_INLINE f32 GetAngleBTWNormalized(const Vec2d& _v1, const Vec2d& _v2)
    {
        return - atan2f(_v1.dot(Turn90(_v2)), _v1.dot(_v2));
    }

    bool PrefetchSkeleton( const Path& path, PrefetchFactory::Params& params )
    {
        return ResourceManager::PrefetchResource( path, params, Resource::ResourceType_AnimSkeleton );
    }

    AnimSkeleton::AnimSkeleton() : m_PolylineBank(NULL)
    {
        m_flushTime = 0;
        m_cookCallback = m_funcCookerSkeleton;
    }

    AnimSkeleton::~AnimSkeleton()
    {
        clear();
    }

    void AnimSkeleton::clear()
    {
        m_BonesCurFrame.clear();
        SF_DEL(m_PolylineBank);
    }


    void AnimSkeleton::serialize(ArchiveMemory& _pArchive, bbool )
    {
        //resource..
        Versioning::serializeVersion(_pArchive,Versioning::animSkeletonVer());

        m_NameToBone.serialize(_pArchive);
        m_UIDToBone.serialize(_pArchive);

		m_BonesCurFrame.serialize(_pArchive);
        AnimBone::restoreParentLink(&m_BonesCurFrame, _pArchive);
        m_BonesTPauseDyn.serialize(_pArchive);

        _pArchive.serialize(m_flushTime);

        bbool hasPolyline = bfalse;
        if (_pArchive.isReading())
        {
            _pArchive.serialize(hasPolyline);
            if (hasPolyline)
            {
                m_PolylineBank = newAlloc(mId_Animation, AnimPolylineBank);
                m_PolylineBank->serialize(_pArchive);
            }
        } else
        {
            hasPolyline = m_PolylineBank != NULL;
            _pArchive.serialize(hasPolyline);
            if (m_PolylineBank)
                m_PolylineBank->serialize(_pArchive);
        }
        ComputePolylineFile();
    }

    AnimBone* AnimSkeleton::getBoneAt(i32 _index)
    {
        if ((_index < 0) || (_index >= (i32)m_BonesCurFrame.size()))
            return NULL;

        return &m_BonesCurFrame[_index];
    }

    i32 AnimSkeleton::getBoneIndex(const StringID& _name)
    {
        NamesMap::Iterator it(m_NameToBone);
        it.findKey(_name);
        if (!it.isEnd())
        {
            i32 index = it.second();
            return index;
        }
        return U32_INVALID;
    }

    i32  AnimSkeleton::getRootIndex()
    {
        if (m_BonesOrdered.size() == 0)
            ComputeBonesOrder();

        if (m_BonesOrdered.size() == 0)
            return -1;

        return m_BonesOrdered[0]->getIndex();
    }


    void AnimSkeleton::SetTPause(SafeArray<AnimBoneDyn> *_dyn)
    {
        ITF_ASSERT(_dyn);
        if (_dyn->size())
            ITF_MEMCOPY(&(*_dyn)[0], &m_BonesTPauseDyn[0], _dyn->size()*sizeof(AnimBoneDyn));
    }

#ifdef NO_RUNTIME_LOCK
    void AnimSkeleton::SetTPauseCook(SafeArray<AnimBoneDynWithLock> *_dyn)
    {
        ITF_ASSERT(_dyn);
        u32 count = _dyn->size();
        for (u32 i=0; i<count; i++)
        {
            *((AnimBoneDyn *)&((*_dyn)[i])) = m_BonesTPauseDyn[i];
            (*_dyn)[i].m_LockedIndex = U32_INVALID;
        }
    }
#endif

    void AnimSkeleton::ComputeBonesOrder()
    {
        u32 count = m_BonesCurFrame.size();
        u8* done = new_array(u8,count,mId_Animation);
 
        ITF_MemSet(done, 0, count);
        VectorAnim<AnimBone>::iterator bIter = m_BonesCurFrame.begin();
        for (i32 pos = 0;bIter != m_BonesCurFrame.end(); ++bIter)
            (*bIter).setIndex(pos++);

        m_BonesOrdered.clear();

        for (bIter = m_BonesCurFrame.begin();
            bIter != m_BonesCurFrame.end(); ++bIter)
        {
            i32 index = (*bIter).getIndex();
            if (!done[index])
            {
                done[index] = 1;
                AnimBone    *B = &(*bIter);
                u32         pos = m_BonesOrdered.size();
                m_BonesOrdered.push_back(B);
                while (B && B->m_BoneParent)
                {
                    i32 parentIndex = B->m_BoneParent->getIndex();
                    if (!done[parentIndex])
                    {
                        done[parentIndex] = 1;
                        B = B->m_BoneParent;
                        m_BonesOrdered.insert(B, pos);
                    } else
                        B = NULL;
                }
            }
        }
        // seach for bone name "root"
        NamesMap::Iterator it(m_NameToBone);
        it.findKey(ITF_GET_STRINGID_CRC(root,170057116));

        if (!it.isEnd())
        {
            i32 index = it.second();
            for (u32 i=0; i<m_BonesOrdered.size(); ++i)
            {
                if (m_BonesOrdered[i]->getIndex() == index)
                {
                    AnimBone* swapBone = m_BonesOrdered[i];
                    m_BonesOrdered[i] = m_BonesOrdered[0];
                    m_BonesOrdered[0] = swapBone;
                    break;
                }
            }
        }

        count = m_BonesOrdered.size();
        
        for (u32 i=0; i<count; ++i)
        {
            AnimBone * pBone = m_BonesOrdered[i];
            pBone->m_OrderedIndex = i;
        }

        SF_DEL_ARRAY(done);
    }

    void AnimSkeleton::ComputeBonesEnd(SafeArray<AnimBoneDyn> *_dyn, bbool _processInvert, bbool _minimalBonesUpdate)
    {
        if (_processInvert)
        {
            if (_minimalBonesUpdate)
            {
                const u32 count = m_BonesMinimumUpdateOrdered.size();
                for (u32 minIdx=0; minIdx<count; ++minIdx)
                {
                    AnimBoneDyn *pBoneDyn = &(*_dyn)[m_BonesMinimumUpdateOrdered[minIdx]->getIndex()];
                    pBoneDyn->ComputeEnd();

                    pBoneDyn->m_Pos.m_y    = 1.f - pBoneDyn->m_Pos.m_y;
                    pBoneDyn->m_PosEnd.m_y = 1.f - pBoneDyn->m_PosEnd.m_y;
                }
            } else
            {
                const u32 count = _dyn->size();
                AnimBoneDyn * pBoneDyn = &(*_dyn)[0];
                for (u32 i=0; i<count; ++i, ++pBoneDyn)
                {
                    pBoneDyn->ComputeEnd();

                    pBoneDyn->m_Pos.m_y    = 1.f - pBoneDyn->m_Pos.m_y;
                    pBoneDyn->m_PosEnd.m_y = 1.f - pBoneDyn->m_PosEnd.m_y;
                }
            }
        } else
        {
            if (_minimalBonesUpdate)
            {
                const u32 count = m_BonesMinimumUpdateOrdered.size();
                for (u32 minIdx=0; minIdx<count; ++minIdx)
                {
                    AnimBoneDyn *pBoneDyn = &(*_dyn)[m_BonesMinimumUpdateOrdered[minIdx]->getIndex()];
                    pBoneDyn->ComputeEnd();
                }

            } else
            {
                const u32 count = _dyn->size();
                for (u32 i=0; i<count; ++i)
                {
                    AnimBoneDyn * pBoneDyn = &(*_dyn)[i];
                    pBoneDyn->ComputeEnd();
                }
            }
        }
    }

#ifndef NO_RUNTIME_LOCK
    void AnimSkeleton::ComputeBonesGlobalPos(SafeArray<AnimBoneDyn> *_dyn, ITF_VECTOR<AnimLock> *_lock, bbool _minimalBonesUpdate)
#else
    void AnimSkeleton::ComputeBonesGlobalPosCook(SafeArray<AnimBoneDynWithLock> *_dyn, ITF_VECTOR<AnimLock> *_lock, bbool _minimalBonesUpdate)
#endif
    {
        ITF_ASSERT(_dyn && _lock);

        if (m_BonesOrdered.size() == 0)
            ComputeBonesOrder();

        SafeArray<AnimBone*> & bonesOrdered = _minimalBonesUpdate ? m_BonesMinimumUpdateOrdered : m_BonesOrdered;
        const u32 count = bonesOrdered.size();
        ITF_ASSERT(count);

        AnimBone **ppBone = &bonesOrdered[0];
        for (u32 i=0; i<count; ++i, ++ppBone)
        {
            AnimBone *pBone = *ppBone;
            AnimBoneDynWithLock &rAnimBone = (*_dyn)[pBone->getIndex()];
            
            if (pBone->m_BoneParent)
            {
                AnimBoneDynWithLock &rAnimBoneParent = (*_dyn)[pBone->m_BoneParent->getIndex()];
                f32         *anglePtr        = NULL;
                
                if (rAnimBoneParent.m_LockedIndex != U32_INVALID)
                {
                    anglePtr = &(*_lock)[rAnimBoneParent.m_LockedIndex].m_LockedAngle;
                }

#ifndef NO_RUNTIME_LOCK
                rAnimBone.ComputeGlobal(&rAnimBoneParent, anglePtr);
#else
                rAnimBone.ComputeGlobalWithLock(&rAnimBoneParent, anglePtr);
#endif
            }
            else
            {
                rAnimBone.ComputeGlobal();
            }
        }
    }


#ifdef NO_RUNTIME_LOCK
    void AnimSkeleton::ComputeBonesGlobalPos(SafeArray<AnimBoneDyn> *_dyn, bbool _minimalBonesUpdate)
    {
        ITF_ASSERT(_dyn);

        if (m_BonesOrdered.size() == 0)
            ComputeBonesOrder();

        SafeArray<AnimBone*> & bonesOrdered = _minimalBonesUpdate ? m_BonesMinimumUpdateOrdered : m_BonesOrdered;
        const u32 count = bonesOrdered.size();
        ITF_ASSERT(count);

        AnimBone **ppBone = &bonesOrdered[0];
        for (u32 i=0; i<count; ++i, ++ppBone)
        {
            AnimBone *pBone = *ppBone;
            AnimBoneDyn &rAnimBone = (*_dyn)[pBone->getIndex()];

            if (pBone->m_BoneParent)
            {
                rAnimBone.ComputeGlobal(&(*_dyn)[pBone->m_BoneParent->getIndex()]);
            }
            else
            {
                rAnimBone.ComputeGlobal();
            }
        }
    }
#endif

    void AnimSkeleton::ComputeBonesLocalPos(SafeArray<AnimBoneDyn> *_dyn)
    {
        if (m_BonesOrdered.size() == 0)
            ComputeBonesOrder();

        const u32 count = m_BonesOrdered.size();
        ITF_ASSERT(count);
        AnimBone * pBone = m_BonesOrdered[0];
        for (u32 i=0; i<count; ++i, ++pBone)
        {
            AnimBoneDyn &rAnimBone = (*_dyn)[pBone->getIndex()];
            if (pBone->m_BoneParent)
            {
                rAnimBone.ComputeLocal(&(*_dyn)[pBone->m_BoneParent->getIndex()]);
            }
            else
                rAnimBone.ComputeLocal(NULL);
        }
    }

    i32 ordersort(AnimLock AL1, AnimLock AL2)
    {
        return AL1.m_details.m_OrderedIndex > AL2.m_details.m_OrderedIndex;
    }
    
    ITF_INLINE bbool resolveIntercection(Vec2d & res1, Vec2d & res2, f32 x0, f32 y0, f32 r0, f32 x1, f32 y1, f32 r1  )
    {
        const f32 r0r0 = r0*r0;
        const f32 r1r1 = r1*r1;
        const f32 N = (r1r1 - r0r0 - x1 * x1 + x0 * x0 - y1 * y1 + y0 * y0) / (2.0f * (y0 - y1));
        const f32 m = (x0 - x1) / (y0 - y1);
        const f32 QA = m * m + 1;
        const f32 QB = 2.0f * y0 * m - 2.0f * N * m - 2 * x0;
        const f32 QC = x0 * x0 + y0 * y0 + N * N - r0r0 - 2.0f * y0 * N;
        const f32 QD = QB * QB - 4.0f * QA * QC;

        if(QD >= 0.0f)
        {
            const f32 inv2QA = f32_Inv(2.0f * QA);
            const f32 sqrtQD = f32_Sqrt(QD);

            res1.m_x = (-QB - sqrtQD) * inv2QA;
            res2.m_x = (-QB + sqrtQD) * inv2QA;

            res1.m_y = N - res1.m_x * m;
            res2.m_y = N - res2.m_x * m;

            return btrue;
        }
        return bfalse;
    }

    void AnimSkeleton::ComputeIK(SafeArray<AnimBoneDynWithLock> *_dyn, ITF_VECTOR<AnimLock> *_lock)
    {
        ITF_ASSERT(_dyn && _lock);
        if (!_lock->size())
            return;

        // Copy of the list IS NOT an option
        ITF_VECTOR<AnimLock> SortedLock = *_lock;

        std::sort(SortedLock.begin(), SortedLock.end(), ordersort);

        for(ITF_VECTOR<AnimLock>::const_iterator sortedLockIter = SortedLock.begin();
            sortedLockIter != SortedLock.end(); ++sortedLockIter)
        {
            const AnimLock*       BLock = &(*sortedLockIter);
            const AnimBone*       B     = &m_BonesCurFrame[BLock->m_details.m_Index];
            ITF_ASSERT(B);
            AnimBoneDyn*    BDyn = &(*_dyn)[BLock->m_details.m_Index];

            const Vec2d& lockPos = BLock->m_Locked;
            const Vec2d dirToLockPosNormalized = (lockPos - BDyn->m_Pos).normalize();

            if(!B->m_BoneParent)
            {
                f32 diff = GetAngleBTWNormalized(Vec2d::XAxis, dirToLockPosNormalized) - BDyn->m_Angle;
                diff = canonizeAngle(diff);
                BDyn->m_Angle += diff;
#ifdef NO_COMPUTECOSSIN
                Vec2d::CosSinOpt(&BDyn->m_CosSin, BDyn->m_Angle);
#endif
                BDyn->m_PosLocal   = lockPos - BDyn->GetLocalEnd();
                BDyn->m_AngleLocal = BDyn->m_Angle;
            }
            else
            {
                AnimBone*       P = B->m_BoneParent;
                AnimBoneDyn*    PDyn = &(*_dyn)[P->getIndex()];
                ITF_ASSERT(P);
                f32 diff, diff0, diff1;
                Vec2d NewP0, NewP1;
                bbool pO1Defined = bfalse;

                Vec2d tmp0( BDyn->m_PosLocal.m_x + PDyn->m_Length, BDyn->m_PosLocal.m_y);
                Vec2d::Mul(&tmp0, &tmp0, &PDyn->m_Scale);

                const f32 r0 = tmp0.norm();
                const f32 r1 = BDyn->GetScaledLength();
                if (f32_Abs(PDyn->m_Pos.m_y - lockPos.m_y) >= f32_Abs(PDyn->m_Pos.m_x - lockPos.m_x))
                {
                    pO1Defined = resolveIntercection(NewP0, NewP1, 
                        PDyn->m_Pos.m_x, PDyn->m_Pos.m_y, r0,
                        lockPos.m_x, lockPos.m_y, r1);
                } else
                {
                    pO1Defined = resolveIntercection(NewP0, NewP1, 
                        PDyn->m_Pos.m_y, PDyn->m_Pos.m_x, r0,
                        lockPos.m_y, lockPos.m_x, r1);
                    if (pO1Defined)
                    {
                        Vec2d::SwapXY(&NewP0, &NewP0);
                        Vec2d::SwapXY(&NewP1, &NewP1);
                    }
                }

                const Vec2d localBoneDirFromParentNormalized = tmp0.normalize();

                if(pO1Defined)
                {
                    diff0 = GetAngleBTWNormalized(localBoneDirFromParentNormalized, (NewP0 - PDyn->m_Pos).normalize());
                    diff1 = GetAngleBTWNormalized(localBoneDirFromParentNormalized, (NewP1 - PDyn->m_Pos).normalize());

                    Vec2d newPos;
                    f32 newPAngle, newAngle, newAngleLocal;
                    if(BLock->m_details.m_AngleSign == 0)
                    {
                        diff0 = canonizeAngle(diff0 - PDyn->m_Angle);
                        diff1 = canonizeAngle(diff1 - PDyn->m_Angle);

                        if(f32_Abs(diff0) < f32_Abs(diff1))
                        {
                            newPos = NewP0;
                            newPAngle = PDyn->m_Angle + diff0;
                        }
                        else
                        {
                            newPos = NewP1;
                            newPAngle = PDyn->m_Angle + diff1;
                        }
                    }
                    else
                    {
                        newPAngle = PDyn->m_Angle + canonizeAngle(diff0 - PDyn->m_Angle);
                        newPos = NewP0;
                        newAngle = GetAngleBTWNormalized(Vec2d::XAxis, (lockPos - newPos).normalize()) - BDyn->m_Angle;
                        newAngle = canonizeAngle(newAngle);
                        newAngle += BDyn->m_Angle;
                        newAngleLocal = canonizeAngle(newAngle - newPAngle);

                        if ((newAngleLocal > 0.f && (BLock->m_details.m_AngleSign & 0x2)) ||
                            (newAngleLocal < 0.f && (BLock->m_details.m_AngleSign & 0x1)))
                        {
                            newPos        = NewP1;
                            newPAngle    = PDyn->m_Angle + canonizeAngle(diff1 - PDyn->m_Angle);
                        }
                    }

                    BDyn->m_Pos = newPos;
                    PDyn->m_Angle = newPAngle;
#ifdef NO_COMPUTECOSSIN
                    Vec2d::CosSinOpt(&PDyn->m_CosSin, PDyn->m_Angle);
#endif
                    diff = GetAngleBTWNormalized(Vec2d::XAxis, (lockPos - newPos).normalize()) - BDyn->m_Angle;
                    diff = canonizeAngle(diff);
                    BDyn->m_Angle += diff;
#ifdef NO_COMPUTECOSSIN
                    Vec2d::CosSinOpt(&BDyn->m_CosSin, BDyn->m_Angle);
#endif
                }
                else
                {
                    // B and P are aligned : use vector

                    const Vec2d dir = (PDyn->m_Pos - lockPos).normalize();
                    f32 way0 = 1;
                    f32 way1 = 1;

                    if((PDyn->m_Pos - BDyn->m_Pos).dot(dir) < 0.0f)
                        way0 = -1.0f;
                    if((BDyn->m_Pos - lockPos).dot(dir) < 0.0f)
                        way1 = -1.0f;

                    BDyn->m_Pos = lockPos + dir * (r1 * way1);
                    diff = GetAngleBTWNormalized(Vec2d::XAxis, dirToLockPosNormalized) - BDyn->m_Angle;
                    diff = canonizeAngle(diff);
                    BDyn->m_Angle += diff;

                    PDyn->m_Pos = BDyn->m_Pos + dir * (r0 * way0);
                    diff = GetAngleBTWNormalized(localBoneDirFromParentNormalized, (BDyn->m_Pos - PDyn->m_Pos).normalize()) - PDyn->m_Angle;
                    diff = canonizeAngle(diff);
                    PDyn->m_Angle += diff;
#ifdef NO_COMPUTECOSSIN
                    Vec2d::CosSinOpt(&PDyn->m_CosSin, PDyn->m_Angle);
#endif
                }

                if(P->m_BoneParent)
                {
                    AnimBoneDyn*    PPDyn = &(*_dyn)[P->m_BoneParent->getIndex()];
                    PDyn->m_AngleLocal = PDyn->m_Angle - PPDyn->m_Angle;
                }
                else
                {
                    PDyn->m_AngleLocal = PDyn->m_Angle;
                }
                BDyn->m_AngleLocal = BDyn->m_Angle - PDyn->m_Angle;
            }
        }
    }

    void AnimSkeleton::Translate(SafeArray<AnimBoneDyn> *_dyn, const Vec2d &_move, AnimGlobalData & _globalData, bbool _minimalBonesUpdate)
    {
        if (_move.m_x == 0.f && _move.m_y == 0.f) return;

        Vec2d::Add(&_globalData.m_origin, &_move, &_globalData.m_origin);
        Vec2d::Add(&_globalData.m_xAxis,  &_move, &_globalData.m_xAxis);
        Vec2d::Add(&_globalData.m_yAxis , &_move, &_globalData.m_yAxis);

        if (_minimalBonesUpdate)
        {
            const u32 count = m_BonesMinimumUpdateOrdered.size();
            for (u32 minIdx=0; minIdx<count; ++minIdx)
            {
                AnimBoneDyn *pBoneDyn = &(*_dyn)[m_BonesMinimumUpdateOrdered[minIdx]->getIndex()];
                Vec2d::Add(&pBoneDyn->m_Pos,    &_move, &pBoneDyn->m_Pos);
                Vec2d::Add(&pBoneDyn->m_PosEnd, &_move, &pBoneDyn->m_PosEnd);
            }
        } else
        {
            const u32 count = _dyn->size();
            for (u32 i=0; i<count; ++i)
            {
                AnimBoneDyn *pBoneDyn = &(*_dyn)[i];
                Vec2d::Add(&pBoneDyn->m_Pos,    &_move, &pBoneDyn->m_Pos);
                Vec2d::Add(&pBoneDyn->m_PosEnd, &_move, &pBoneDyn->m_PosEnd);
            }
        }
    }

    void AnimSkeleton::Scale(SafeArray<AnimBoneDyn> *_dyn, const Vec2d &_scale, AnimGlobalData & _globalData, bbool _minimalBonesUpdate)
    {
        if (_scale.m_x == 1.f && _scale.m_y == 1.f)
            return;
        if (_scale.m_x * _scale.m_y < 0.f)
            _globalData.m_mirror = !_globalData.m_mirror;

        Vec2d::Mul(&_globalData.m_origin, &_scale, &_globalData.m_origin);
        Vec2d::Mul(&_globalData.m_xAxis, &_scale, &_globalData.m_xAxis);
        Vec2d::Mul(&_globalData.m_yAxis, &_scale, &_globalData.m_yAxis);

        if (_minimalBonesUpdate)
        {
            const u32 count = m_BonesMinimumUpdateOrdered.size();
            for (u32 minIdx=0; minIdx<count; ++minIdx)
            {
                AnimBoneDyn *pBoneDyn = &(*_dyn)[m_BonesMinimumUpdateOrdered[minIdx]->getIndex()];
                Vec2d::Mul(&pBoneDyn->m_Pos,    &_scale, &pBoneDyn->m_Pos);
                Vec2d::Mul(&pBoneDyn->m_PosEnd, &_scale, &pBoneDyn->m_PosEnd);
            }
        } else
        {
            const u32 count = _dyn->size();
            for (u32 i=0; i<count; ++i)
            {
                AnimBoneDyn *pBoneDyn = &(*_dyn)[i];
                Vec2d::Mul(&pBoneDyn->m_Pos,    &_scale, &pBoneDyn->m_Pos);
                Vec2d::Mul(&pBoneDyn->m_PosEnd, &_scale, &pBoneDyn->m_PosEnd);
            }
        }
    }

    void AnimSkeleton::Rotate(SafeArray<AnimBoneDyn> *_dyn, f32 _angle, AnimGlobalData & _globalData, bbool _minimalBonesUpdate)
    {
        if (_angle == 0.f) return;
        if (m_BonesOrdered.size() == 0)
            ComputeBonesOrder();

        Vec2d angleCosSin;
        Vec2d::CosSinOpt(&angleCosSin, _angle);

        _globalData.m_origin= _globalData.m_origin.RotateCS(angleCosSin);
        _globalData.m_xAxis = _globalData.m_xAxis.RotateCS(angleCosSin);
        _globalData.m_yAxis = _globalData.m_yAxis.RotateCS(angleCosSin);

        AnimBoneDyn *mainBone = &(*_dyn)[m_BonesOrdered[0]->getIndex()];
        if (mainBone->m_Pos.sqrnorm() < MTH_EPSILON)
        {
            if (_minimalBonesUpdate)
            {
                const u32 count = m_BonesMinimumUpdateOrdered.size();
                for (u32 minIdx=0; minIdx<count; ++minIdx)
                {
                    AnimBoneDyn *pBoneDyn = &(*_dyn)[m_BonesMinimumUpdateOrdered[minIdx]->getIndex()];
                    pBoneDyn->m_Pos    = pBoneDyn->m_Pos.RotateCS(angleCosSin);
                    pBoneDyn->m_PosEnd = pBoneDyn->m_PosEnd.RotateCS(angleCosSin);
                }
            } else
            {
                const u32 count = _dyn->size();
                for (u32 i=0; i<count; ++i)
                {
                    AnimBoneDyn *pBoneDyn = &(*_dyn)[i];
                    pBoneDyn->m_Pos    = pBoneDyn->m_Pos.RotateCS(angleCosSin);
                    pBoneDyn->m_PosEnd = pBoneDyn->m_PosEnd.RotateCS(angleCosSin);
                }
            }
        } else
        {
            if (_minimalBonesUpdate)
            {
                const u32 count = m_BonesMinimumUpdateOrdered.size();
                for (u32 minIdx=0; minIdx<count; ++minIdx)
                {
                    AnimBoneDyn *pBoneDyn = &(*_dyn)[m_BonesMinimumUpdateOrdered[minIdx]->getIndex()];
                    pBoneDyn->m_Pos    = pBoneDyn->m_Pos.RotateAroundCS(mainBone->m_Pos, angleCosSin);
                    pBoneDyn->m_PosEnd = pBoneDyn->m_PosEnd.RotateAroundCS(mainBone->m_Pos, angleCosSin);
                }
            } else
            {
                const u32 count = _dyn->size();
                for (u32 i=0; i<count; ++i)
                {
                    AnimBoneDyn *pBoneDyn = &(*_dyn)[i];
                    pBoneDyn->m_Pos    = pBoneDyn->m_Pos.RotateAroundCS(mainBone->m_Pos, angleCosSin);
                    pBoneDyn->m_PosEnd = pBoneDyn->m_PosEnd.RotateAroundCS(mainBone->m_Pos, angleCosSin);
                }
            }
        }
    }


    void AnimSkeleton::RotateBone(SafeArray<AnimBoneDyn> *_dyn, i32 _boneIndex, f32 _angle)
    {
        if (_angle == 0.f || _boneIndex < 0 || _boneIndex >= (i32)_dyn->size()) return;
        if (m_BonesOrdered.size() == 0)
            ComputeBonesOrder();

        f32 _angleCos, _angleSin;
        f32_CosSinOpt(_angle, &_angleCos, &_angleSin);
        
        const u32 count = _dyn->size();

        char *parentBuffer = newAlloc(mId_Animation, char[count]);
        ITF_MemSet(parentBuffer, 0, count);
        parentBuffer[_boneIndex] = 1;

        AnimBoneDyn *mainBone = &(*_dyn)[_boneIndex];

        mainBone->m_PosEnd      = mainBone->m_PosEnd.RotateAroundCS(mainBone->m_Pos, _angleCos, _angleSin);

        const i32 orderSize = (i32)m_BonesOrdered.size();
        for (i32 orderIdx = m_BonesCurFrame[_boneIndex].m_OrderedIndex + 1;
             orderIdx < orderSize; ++orderIdx)
        {
            AnimBone *bone = m_BonesOrdered[orderIdx];
            if ( (   bone->m_BoneParent &&
                      parentBuffer[bone->m_BoneParent->getIndex()]))
            {
                i32 index = bone->getIndex();
                AnimBoneDyn *pBoneDyn   = &(*_dyn)[index];
                pBoneDyn->m_Pos         = pBoneDyn->m_Pos.RotateAroundCS(mainBone->m_Pos, _angleCos, _angleSin);
                pBoneDyn->m_PosEnd      = pBoneDyn->m_PosEnd.RotateAroundCS(mainBone->m_Pos, _angleCos, _angleSin);
                parentBuffer[index]     = 1;
            }
        }
        SF_DEL_ARRAY(parentBuffer);
    }


    void AnimSkeleton::SwapHorizontal(SafeArray<AnimBoneDyn> *_dyn, AnimGlobalData & _globalData, bbool _minimalBonesUpdate)
    {
        _globalData.m_mirror = !_globalData.m_mirror;
        _globalData.m_origin.m_x   *= -1.0f;
        _globalData.m_xAxis.m_x    *= -1.0f;
        _globalData.m_yAxis.m_x    *= -1.0f;

        if (_minimalBonesUpdate)
        {
            const u32 count = m_BonesMinimumUpdateOrdered.size();
            for (u32 minIdx=0; minIdx<count; ++minIdx)
            {
                AnimBoneDyn *pBoneDyn = &(*_dyn)[m_BonesMinimumUpdateOrdered[minIdx]->getIndex()];
                pBoneDyn->m_Pos.m_x    *= -1.0f;
                pBoneDyn->m_PosEnd.m_x *= -1.0f;
            }
        } else
        {
            const u32 count = _dyn->size();
            for (u32 i=0; i<count; ++i)
            {
                AnimBoneDyn *pBoneDyn = &(*_dyn)[i];
                pBoneDyn->m_Pos.m_x    *= -1.0f;
                pBoneDyn->m_PosEnd.m_x *= -1.0f;
            }
        }
    }


    i32 AnimSkeleton::GetChildrenOf(u32 _index, SafeArray<char> &boneCheckBuffer)
    {
        if (_index >= m_BonesCurFrame.size())
            return 0;

        if (m_BonesOrdered.size() == 0)
            ComputeBonesOrder();

        const i32 orderSize = (i32)m_BonesOrdered.size();
        boneCheckBuffer.resize(orderSize);
        ITF_MemSet(&boneCheckBuffer[0], 0, orderSize);
        
        i32 count = 1;
        boneCheckBuffer[_index] = 1;

        for (i32 orderIdx = m_BonesCurFrame[_index].m_OrderedIndex + 1;
            orderIdx < orderSize; ++orderIdx)
        {
            AnimBone *bone = m_BonesOrdered[orderIdx];
            if (bone->m_BoneParent &&
                boneCheckBuffer[bone->m_BoneParent->getIndex()])
            {
                boneCheckBuffer[bone->getIndex()] = 1;
                count++;
            }
        }
        return count;
    }


    bbool AnimSkeleton::getPolyline(StringID _polyID, AnimPolyline * &_pPolyline)
    {
        if (!m_PolylineBank)
            return bfalse;

        NamesMap::Iterator it(m_PolylineBank->m_NameToPolyline);
        it.findKey(_polyID);
        if (it.isEnd() || it.second() >= (i32)m_PolylineBank->m_PolylineList.size())
            return bfalse;

        _pPolyline = &m_PolylineBank->m_PolylineList[it.second()];
        return btrue;
    }

    void AnimSkeleton::ComputePolylineFile()
    {
        if ( m_PolylineBank != NULL )
            m_PolylineBank->resolvePtr( this );
    }

    void AnimSkeleton::postProcess(const String& _szFilename, const String& _szPlatform, bbool _tryLoadingBin)
    {
        ComputeBonesOrder();
        ComputePolylineFile();
        ProcessMinimalBonesUpdateList();
    }

    void AnimSkeleton::ProcessMinimalBonesUpdateList()
    {
        m_BonesMinimumUpdateOrdered.clear();

        if (m_BonesOrdered.size() == 0)
            ComputeBonesOrder();

        if (m_BonesOrdered.size() == 0)
            return;

        // always update root
        m_BonesMinimumUpdateOrdered.push_back(m_BonesOrdered[0]);
        m_BonesOrdered[0]->m_PatchFlags |= BONE_MINIMAL_COMPUTE;

        if (!m_PolylineBank)
            return;
            
        VectorAnim<AnimPolyline>::iterator polyIter = m_PolylineBank->m_PolylineList.begin();
        for (; polyIter != m_PolylineBank->m_PolylineList.end(); ++polyIter)
        {
            AnimPolyline & poly = *polyIter;
            
            VectorAnim<AnimPolylinePoint>::iterator polyPtIter = poly.m_PolylinePointList.begin();
            for (; polyPtIter != poly.m_PolylinePointList.end(); ++polyPtIter)
            {
                AnimBone * pBone = (*polyPtIter).m_BonePtr;
                u32         pos  = m_BonesMinimumUpdateOrdered.size();
                while (pBone && !(pBone->m_PatchFlags & BONE_MINIMAL_COMPUTE))
                {
                    m_BonesMinimumUpdateOrdered.insert(pBone, pos);
                    pBone->m_PatchFlags |= BONE_MINIMAL_COMPUTE;
                    pBone = pBone->m_BoneParent;
                }
            }
        }
    }

    bbool AnimSkeleton::isBoneInMinimalBonesList(i32 _index )
    {
        u32 count = m_BonesMinimumUpdateOrdered.size();
        for (u32 bIdx=0; bIdx<count; ++bIdx)
        {
            if (m_BonesMinimumUpdateOrdered[bIdx]->getIndex() == _index)
                return btrue;
        }
        return bfalse;
    }


};
