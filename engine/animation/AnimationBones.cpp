#include "precompiled_engine.h"

#ifndef _ITF_ANIMATIONBONES_H_
#include    "engine/animation/AnimationBones.h"
#endif // _ITF_ANIMATIONBONES_H_

#ifndef _ITF_ANIMATIONPATCHBANK_H_
#include    "engine/animation/AnimationPatchBank.h"
#endif // _ITF_ANIMATIONPATCHBANK_H_

#ifndef _ITF_ARCHIVELINKER_H_
#include "core/ArchiveLinker.h"
#endif //_ITF_ARCHIVELINKER_H_

namespace ITF
{
    
    AnimBoneDyn::AnimBoneDyn() :
     m_Length(0.001f)
#ifndef NO_RUNTIME_LOCK
     , m_LockedIndex (U32_INVALID)// if the bone is locked give index on lock list, else, -1
#endif
    , m_AngleLocal(0.f)
    , m_Alpha(0.f)
    , m_Zorder(0.f)
	, m_PosEnd(Vec2d::Zero)
    , m_CosSin(Vec2d::Zero)
    , m_Pos(0.0f,0.001f)
    , m_PosLocal(Vec2d::Zero)
    , m_Scale(Vec2d::One)
#ifndef NO_COMPUTECOSSIN
    , m_CosSinAngle(F32_INFINITY)
#endif
    {
    }

    void AnimBoneDyn::serialize(ArchiveMemory &_archive)
    {
        m_Pos.serialize(_archive);
        _archive.serialize(m_Angle);
        _archive.serialize(m_Length);

        m_PosLocal.serialize(_archive);
        _archive.serialize(m_AngleLocal);

        _archive.serialize(m_Zorder);
        m_Scale.serialize(_archive);
        _archive.serialize(m_Alpha);
    }

    void AnimBone::serialize(ArchiveMemory& _archive)
    {
        _archive.serializeLink(reinterpret_cast<uPtr>(this));

        m_Name.serialize(_archive);
        _archive.serialize(m_PatchFlags);

        u32 size = m_PatchPointsPtr.size();
        _archive.serialize(size);
        if (_archive.isReading())
        {        
            m_PatchPointsPtr.resize(size);
            for (u32 index =0;index<size;index++)
            {
                _archive.serialize((void *&)m_PatchPointsPtr[index]);
            }
        }
        else
        {
            size =  m_PatchPointsPtr.size();
            for ( VectorAnim<AnimPatchPoint*>::iterator iter = m_PatchPointsPtr.begin();iter!=m_PatchPointsPtr.end();++iter)
            {         
                _archive.serialize((void *&)(*iter));
            }
        }

        _archive.serialize((void*&)m_BoneParent);//will be reconstruct later
    }

    void   AnimBone::link(AnimPatchBank *_animPatchBank)
    {
        ArchiveLinker *archiveLinker = _animPatchBank->m_archiveLinker;
        u32 size = m_PatchPointsPtr.size();
        for (u32 index =0;index<size;index++)
        {
            m_PatchPointsPtr[index] = (AnimPatchPoint *)archiveLinker->getLink(uPtr(m_PatchPointsPtr[index]));
        }    
    }


    void  AnimBone::restoreParentLink(VectorAnim<AnimBone> *_animBoneList, ArchiveMemory& _Archive)
    {
        if (_Archive.isReading())
        {        
			u32 size = _animBoneList->size();
            for (u32 index =0;index<size;index++)
            {
                AnimBone* pBoneParent = (AnimBone*) _Archive.getLinker()->getLink(uPtr(((*_animBoneList)[index].m_BoneParent)));
                (*_animBoneList)[index].m_BoneParent = pBoneParent;
            }
        }
    }

    void AnimLock::serialize(ArchiveMemory& _archive)
    {
        _archive.serialize(m_serializeField);
        _archive.serialize(m_LockedAngle);
        m_Locked.serialize(_archive);
    }

};

