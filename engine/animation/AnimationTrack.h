#ifndef _ITF_ANIMATIONTRACK_H_
#define _ITF_ANIMATIONTRACK_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_BV_AABB_H_
#include "core/boundingvolume/AABB.h"
#endif //_ITF_BV_AABB_H_

#ifndef _ITF_SAFEARRAY_H_
#include "core/container/SafeArray.h"
#endif //_ITF_SAFEARRAY_H_

#ifndef _ITF_FASTARRAY_H_
#include "core/container/FastArray.h"
#endif //_ITF_FASTARRAY_H_

#ifndef _ITF_ANIMATIONBONES_H_
#include    "engine/animation/AnimationBones.h"
#endif // _ITF_ANIMATIONBONES_H_

#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_

#ifndef _ITF_ANIMATION_SERIALIZE_H_
#include "engine/animation/AnimationSerialize.h"
#endif //_ITF_ANIMATION_SERIALIZE_H_

#ifndef _ITF_PUGIXMLWRAP_H_
#include "core/XML/PugiXMLWrap.h"
#endif //_ITF_PUGIXMLWRAP_H_

#ifdef ITF_WII
#define ITF_COMPRESSED_ANIM
#endif //ITF_WII

namespace ITF
{  
    class AnimTemplate;
    class AnimSkeleton;
    class AnimPatchBank;
    class AnimPatch;
    class AnimLock;
    class AnimTrackExt;
    class AnimInfo;
    class AnimPatchZ;

	////////////////////////////////////////////////////////////////
	///Animation key
    class AnimTrackBoneWithLock
    {
    public:
        AnimTrackBoneWithLock() :  //TODO Yous please check the useless constructor
          m_Lock(NULL)
              , m_frame(0.f)
              , m_Alpha(0.f)
              , m_AngleLocal(0.f)
              , m_Zorder(0.f)
              , m_Scale(Vec2d::One)
              , m_PosLocal(Vec2d::Zero)
          {
          }
        AnimTrackBoneWithLock(const AnimTrackBoneWithLock &_trk);
        ~AnimTrackBoneWithLock();

        f32                     m_frame;

        Vec2d                   m_PosLocal;
        f32                     m_AngleLocal;

        Vec2d                   m_Scale;
        f32                     m_Zorder;
        f32                     m_Alpha;

        AnimLock                *m_Lock;

        AnimTrackBoneWithLock& operator=(const AnimTrackBoneWithLock& _trk);
    };
    VECTAGSET(AnimTrackBoneWithLock, ITF::MemoryId::mId_VectorAnimTrackBone)
    typedef VectorAnim<AnimTrackBoneWithLock> VectorAnimTrackBoneWithLock;

#ifndef NO_RUNTIME_LOCK
    typedef AnimTrackBoneWithLock AnimTrackBone;
#else
    class AnimTrackBone
    {
    public:
        AnimTrackBone()
            : m_frame(0.f)
            , m_Alpha(0.f)
            , m_AngleLocal(0.f)
            , m_Zorder(0.f)
            , m_Scale(1.f, 1.f)
            , m_PosLocal(Vec2d::Zero)
        { }
        AnimTrackBone(const AnimTrackBone &_trk) { *this = _trk; }

        f32                     m_frame;

        Vec2d                   m_PosLocal;
        f32                     m_AngleLocal;

        Vec2d                   m_Scale;
        f32                     m_Zorder;
        f32                     m_Alpha;
    };
    VECTAGSET(AnimTrackBone, ITF::MemoryId::mId_VectorAnimTrackBone)
    typedef VectorAnim<AnimTrackBone> VectorAnimTrackBone;
#endif

#define RATIO_MULT_I16 32768
#define RATIO_DIV_I16 (1.f/32768.f)

    class AnimTrackBonePAS
    {
    public:
        AnimTrackBonePAS();
        AnimTrackBonePAS(const AnimTrackBonePAS &_trk);
        ~AnimTrackBonePAS();

        void serialize(ArchiveMemory& _archive);
        bbool isEpsilonEqual(AnimTrackBonePAS *_trkBone);

        void                    getPos(Vec2d* _dst, f32 _max) const;
        void                    getScale(Vec2d* _dst, f32 _max) const;        

        f32                     getAngle(f32 _max)    const             { return (_max * m_AngleLocal)*RATIO_DIV_I16;   }
        Vec2d                   getPos(f32 _max)      const             { return Vec2d((_max * m_PosLocalX)*RATIO_DIV_I16, (_max * m_PosLocalY)*RATIO_DIV_I16);}
        Vec2d                   getScale(f32 _max)    const             { return Vec2d((_max * m_ScaleX)*RATIO_DIV_I16, (_max * m_ScaleY)*RATIO_DIV_I16);}

        void                    setAngle(f32 _angle, f32 _max)          { m_AngleLocal  = (i16)((_angle / _max) * RATIO_MULT_I16);   }
        void                    setPos(const Vec2d & _pos, f32 _max)    { m_PosLocalX   = (i16)((_pos.m_x / _max) * RATIO_MULT_I16);    m_PosLocalY = (i16)((_pos.m_y / _max) * RATIO_MULT_I16);   }
        void                    setScale(const Vec2d & _scale, f32 _max){ m_ScaleX      = (i16)((_scale.m_x / _max) * RATIO_MULT_I16);  m_ScaleY    = (i16)((_scale.m_y / _max) * RATIO_MULT_I16);   }

    public:
        u16                     m_frame;
        i16                     m_AngleLocal;

        i16                     m_PosLocalX;
        i16                     m_PosLocalY;

        i16                     m_ScaleX;
        i16                     m_ScaleY;
    };
    VECTAGSET(AnimTrackBonePAS, ITF::MemoryId::mId_VectorAnimTrackBonePAS)
    typedef VectorAnim<AnimTrackBonePAS> VectorAnimTrackBonePAS;

    ITF_INLINE void AnimTrackBonePAS::getPos(Vec2d* _dst, f32 _max) const
    {
        *_dst = Vec2d(m_PosLocalX, m_PosLocalY);
        Vec2d::Scale(_dst, _dst, _max*RATIO_DIV_I16);
    }
    ITF_INLINE void AnimTrackBonePAS::getScale(Vec2d* _dst, f32 _max) const
    {
        *_dst = Vec2d(m_ScaleX, m_ScaleY);
        Vec2d::Scale(_dst, _dst, _max*RATIO_DIV_I16);
    }

    class AnimTrackBoneZAL
    {
    public:
        AnimTrackBoneZAL();
        AnimTrackBoneZAL(const AnimTrackBoneZAL &_trk);
        ~AnimTrackBoneZAL();

        u16                     m_frame;
        i16                     m_Alpha;
        f32                     m_Zorder;

#ifndef NO_RUNTIME_LOCK
        AnimLock                *m_Lock;
#endif

        AnimTrackBoneZAL& operator=(const AnimTrackBoneZAL& __x);
        void serialize(ArchiveMemory& _archive);

        bbool isEpsilonEqual(AnimTrackBoneZAL *_trkBone);
    };

    VECTAGSET(AnimTrackBoneZAL, ITF::MemoryId::mId_VectorAnimTrackBoneZAL)
    typedef VectorAnim<AnimTrackBoneZAL> VectorAnimTrackBoneZAL;
    
    ////////////////////////////////////////////////////////////////////
	/// Animation keys for a bone, for an animation
    class AnimTrackBonesList
    {
    public:
        //VectorAnim<AnimTrackBone>       m_trackList;

        u16                             m_trackPASIndex;
        u16                             m_trackPASSize;

        u16                             m_trackZALIndex;
        u16                             m_trackZALSize;


        void serialize(ArchiveMemory& _pArchive);
        static void setTrack(VectorAnim<AnimTrackBoneWithLock> &_trackList, const AnimTrackBoneWithLock & _track);

        AnimTrackBonesList() :
            m_trackPASIndex(0), m_trackPASSize(0), m_trackZALIndex(0), m_trackZALSize(0)
            {};
    };
    VECTAGSET(AnimTrackBonesList, ITF::MemoryId::mId_VectorAnimation)

	///////////////////////////////////////////////////////////////////////////////////
	///"Bone Matching List" : for a given frame & animation, gives graphic representation of bones
    typedef struct TemplateRef_
    {
        StringID    m_bankName;
        StringID    m_patchName;

        void serialize(ArchiveMemory& _Archive)
        {
            m_bankName.serialize(_Archive);
            m_patchName.serialize(_Archive);
        }
    } TemplateRef;
    VECTAGSET(TemplateRef, ITF::MemoryId::mId_VectorAnimation)

    class AnimTrackBML
    {
    public:
        f32                             m_frame;
        VectorAnim<TemplateRef>             m_templateRefs; // stored
        void serialize(ArchiveMemory& _Archive);

        AnimTrackBML() : m_frame(0.f) {}

        void        templateToDynamicsBonesLen(AnimInfo * _animInfo, const AnimTrackExt * animTrack);
        void        dynamicBonesToTemplate(AnimInfo * _animInfo, const AnimTrackExt * animTrack);
        void        setComputeTab(const AnimTrackExt * animTrack, const AnimFrameInfo::BonesArray& _boneUsed, SafeArray<char> &_templateUsed, u32 _state);
        u32         getCurrentPatchs(const AnimTrackExt *_animTrack, const SafeArray<char> &_templateUsed, SafeArray<AnimPatchZ> &_patchList, u32 _state);
    };
    VECTAGSET(AnimTrackBML, ITF::MemoryId::mId_VectorAnimation)

    class AnimTrackPolyline
    {
    public:
        f32                             m_frame;
        VectorAnim<StringID>                m_polylineRefs; // stored
        void serialize(ArchiveMemory& _Archive);

        AnimTrackPolyline() : m_frame(0.f) {}
        void        dynamicBonesToPolyline(AnimInfo * _animInfo, AnimTrackExt * animTrack);
    };
    VECTAGSET(AnimTrackPolyline, ITF::MemoryId::mId_VectorAnimation)

    enum    AnimEvtType
    {
        AnimEvtType_FX          =   1,
        AnimEvtType_Animation   =   2,
        AnimEvtType_Gameplay    =   3,
        AnimEvtType_Polyline    =   4,
        AnimEvtType_Partition   =   5,
        AnimEvtType_None        =   U32_INVALID,
        ENUM_FORCE_SIZE_32(AnimEvtType)
    };

    class AnimMarkerEvent : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(AnimMarkerEvent,Event,754266589);
        DECLARE_SERIALIZE()

    public:

        AnimMarkerEvent() : m_PosLocal(Vec2d::Zero), m_Pos(Vec2d::Zero), m_boneParent(U32_INVALID) {}
        ~AnimMarkerEvent() {}

        virtual AnimEvtType             getAnimEvtType() const { return AnimEvtType_None; }
        const StringID&                 getName() const { return m_Name; }
        const StringID&                 getFriendly() const { return m_animFriendly; }
        const Vec2d&                    getPosLocal() const { return m_PosLocal; }

        void                            setName( const StringID& _name ) { m_Name = _name; }
        void                            setPosLocal( Vec2d& _pos ) { m_PosLocal = _pos; }
        void                            setPos( Vec2d& _pos ) { m_Pos = _pos; }
        void                            setFriendly( const StringID& _id ) { m_animFriendly = _id; }

        virtual void                    serialize(ArchiveMemory& _pArchive);

        static AnimMarkerEvent*         createEvent( AnimEvtType _type );

#ifdef ITF_SUPPORT_XML
        virtual bbool                   ReadXML(class PugiXMLWrap *pXML, NamesMap *_uidToBone, int errBehavior);
        virtual void                    WriteXML(pugi::xml_node node, AnimSkeleton * skeleton);
#endif //ITF_SUPPORT_XML
        u32                             getBoneParent() { return m_boneParent; }

    private:
        u32                             m_boneParent;
        StringID                        m_Name;
        Vec2d                           m_PosLocal;
        Vec2d                           m_Pos;
        StringID                        m_animFriendly;
    };
    VECTAGSET(AnimMarkerEvent, ITF::MemoryId::mId_VectorAnimation)

    class AnimFXEvent : public AnimMarkerEvent
    {
        DECLARE_OBJECT_CHILD_RTTI(AnimFXEvent,AnimMarkerEvent,3753665187);
        DECLARE_SERIALIZE()

    public:

        enum FXAction
        {
            FX_Stop     = 0,
            FX_Start    = 1
        };
        


        AnimFXEvent() : m_action(FX_Start) {}
        ~AnimFXEvent() {}

        virtual AnimEvtType             getAnimEvtType() const { return AnimEvtType_FX; }
        virtual void                    serialize(ArchiveMemory& _pArchive);
        FXAction                        getPolylineID() { return m_action; }

#ifdef ITF_SUPPORT_XML        
        virtual bbool                   ReadXML(class PugiXMLWrap *pXML, NamesMap *_uidToBone, int errBehavior);
        virtual void                    WriteXML(pugi::xml_node node, AnimSkeleton * skeleton);
#endif //ITF_SUPPORT_XML

        ITF_INLINE FXAction             getAction() const { return m_action; }

    private:
        FXAction    m_action;

    };

    class AnimAnimationEvent : public AnimMarkerEvent
    {
        DECLARE_OBJECT_CHILD_RTTI(AnimAnimationEvent,AnimMarkerEvent,1183342651);
        DECLARE_SERIALIZE()

    public:

        AnimAnimationEvent() {}
        ~AnimAnimationEvent() {}

        virtual AnimEvtType             getAnimEvtType() const { return AnimEvtType_Animation; }
    };

    class AnimGameplayEvent : public AnimMarkerEvent
    {
        DECLARE_OBJECT_CHILD_RTTI(AnimGameplayEvent,AnimMarkerEvent,2720277301);
        DECLARE_SERIALIZE()

    public:

        AnimGameplayEvent() {}
        ~AnimGameplayEvent() {}

        virtual AnimEvtType             getAnimEvtType() const { return AnimEvtType_Gameplay; }
    };

    class AnimPolylineEvent : public AnimMarkerEvent
    {
        DECLARE_OBJECT_CHILD_RTTI(AnimPolylineEvent,AnimMarkerEvent,4083589773);
        DECLARE_SERIALIZE()

    public:

        AnimPolylineEvent() : m_points(NULL), m_numPoints(0) {}
        ~AnimPolylineEvent() {}

        virtual AnimEvtType             getAnimEvtType() const { return AnimEvtType_Polyline; }
        virtual void                    serialize(ArchiveMemory& _pArchive);

        StringID                        getPolylineID() { return m_polylineID; }
        const Vec2d*                    getPoints() const { return m_points; }
        u32                             getNumPoints() const { return m_numPoints; }
        void                            setPoints( const Vec2d* _points, u32 _numPoints );

#ifdef ITF_SUPPORT_XML
        virtual bbool                   ReadXML(class PugiXMLWrap *pXML, NamesMap *_uidToBone, int errBehavior);
        virtual void                    WriteXML(pugi::xml_node node, AnimSkeleton * skeleton);
#endif //ITF_SUPPORT_XML

    private:

        StringID                        m_polylineID;
        const Vec2d*                    m_points;
        u32                             m_numPoints;
    };


    class AnimPartitionEvent : public AnimMarkerEvent
    {
        DECLARE_OBJECT_CHILD_RTTI(AnimPartitionEvent,AnimMarkerEvent,3271664900);
        DECLARE_SERIALIZE()

    public:

        AnimPartitionEvent() : m_percent(-1.f) {}
        ~AnimPartitionEvent() {}

        virtual AnimEvtType             getAnimEvtType() const { return AnimEvtType_Partition; }
        virtual void                    serialize(ArchiveMemory& _pArchive);

#ifdef ITF_SUPPORT_XML
        virtual bbool                   ReadXML(class PugiXMLWrap *pXML, NamesMap *_uidToBone, int errBehavior);
        virtual void                    WriteXML(pugi::xml_node node, AnimSkeleton * skeleton);
#endif //ITF_SUPPORT_XML

        ITF_INLINE f32                  getPercent() const { return m_percent; }

    private:
        f32      m_percent;

    };


    typedef SafeArray<AnimMarkerEvent*,8,MemoryId::mId_SafeArray,btrue,bfalse> AnimEventContainer;

	////////////////////////////////////////////////////////////////////////
	//For a frame of an animation, gives events
    class AnimTrackFrameEvents
    {
    public:
        AnimTrackFrameEvents() : m_frame(0.f) {}
        // The events are not deleted here on purpose, because on some places we are copying them from one array to
        // another
        ~AnimTrackFrameEvents() {}

        f32                             m_frame;
        AnimEventContainer              m_frameEvents;
 
        void                            serialize(ArchiveMemory& _pArchive);
    };
    VECTAGSET(AnimTrackFrameEvents, ITF::MemoryId::mId_VectorAnimation)

    class AnimTrackFrameSoundEvents
    {
    public:
        StringID                    m_name;
        f32                         m_startRatio; //
        u32                         m_parentBoneIndex;

        AnimTrackFrameSoundEvents() : m_startRatio(0.f),m_parentBoneIndex(U32_INVALID)
                                    {}
       
        void                        serialize(ArchiveMemory& _pArchive);
    };
    VECTAGSET(AnimTrackFrameSoundEvents, ITF::MemoryId::mId_VectorAnimation)

	///////////////////////////////////////////////////////////////////////////
	//An animation (run, jump, ...)
#define ANIM_TRACK_USE_ALPHA            0x1
#define ANIM_TRACK_IS_DEFAULT_RIGHT     0x2


    class SubAnimBeginEnd
    {
    public: 
        SubAnimBeginEnd() : m_begin(-1.f), m_end(-1.f) {}

        StringID    m_subAnim;
        f32         m_begin;
        f32         m_end;
    };

    class AnimTrack : public AnimationSerialize
    {

    public:
        static String                   s_redirectPatchBank;

        StringID                        m_name;

        f32                             m_endFrame;
        AABB                            m_bounding;
        AABB                            m_scissor;
        u32                             m_tags;

        AnimSkeleton *                  m_dummySkeleton;        // for local process only!!
        StringID                        m_skeletonPath;         // will work because of ANIM_MANAGER stringID convert
        u64                             m_skeletonFlushTime;
        SafeArray<u64>                  m_redirectPatchBankFlushTimeList;
        ResourceID                      m_skeletonResourceID;

        SafeArray<StringID>             m_patchBankPathList;
        ITF_VECTOR<ResourceID>          m_textureResourceIDList;
        ITF_VECTOR<ResourceID>          m_patchBankResourceIDList;        

        NamesMap                        m_nameToPatch; // taken from resource

        VectorAnim<AnimTrackBonesList>      m_bonesList; // Must be same index as main skeleton m_BonesCurFrame indexes
        
        // now, only one vector for all tracks -> track will index it
		FastArray<AnimTrackBonePAS>			m_trackListPAS;
        VectorAnim<AnimTrackBoneZAL>        m_trackListZAL;

        f32                                 m_bonesMaxAngle;
        f32                                 m_bonesMaxPos;
        f32                                 m_bonesMaxScale;

        VectorAnim<Vec2d>                   m_rootPos;

        VectorAnim<AnimTrackBML>                m_bmlList; //For a set of frame, graphic representation changes of bone sets
        VectorAnim<AnimTrackPolyline>           m_polylineList; // Must be same index as main skeleton m_BonesCurFrame indexes
        VectorAnim<AnimTrackFrameEvents>        m_eventList;

        VectorAnim<AnimTrackFrameSoundEvents>   m_soundList;

#ifdef ITF_COMPRESSED_ANIM
		void*								m_compressedTrackListPAS;
		u32									m_trackListPASSize;
		u32									m_trackListPASCompressedSize;
		void								clearDecompressed() {m_trackListPAS.destroy();}
#endif //ITF_COMPRESSED_ANIM

        void                            processTagAndBounding(bbool saveRootPos, bbool _addSkeletonABB);
        void                            serialize(ArchiveMemory& _Archive, bbool _compressedData);

        ////// anim processing
#ifdef ANIM_TRACK_CACHE
    #ifndef NO_RUNTIME_LOCK
        bbool                           fillTrackAtIndex(AnimTrackBone & track, AnimTrackBonePASCut * _pasCache,  const AnimTrackBonesList & animBoneLst, f32 index, AnimLock *lock );
        void                            ComputeAnimFrame(SafeArray<AnimBoneDyn> *_dyn, SAFEARRAY_CACHE_TRCKBONE_PASCUT & _pasCache, ITF_VECTOR<AnimLock> *_lock, f32 index, f32 percent, bbool isMain, const AnimFrameInfo::BonesArray* _templateUsed, bbool _minimalBones);
    #else
        bbool                           fillTrackAtIndex(AnimTrackBone & track, AnimTrackBonePASCut * _pasCache,  const AnimTrackBonesList & animBoneLst, f32 index );
        void                            ComputeAnimFrame(SafeArray<AnimBoneDyn> *_dyn, SAFEARRAY_CACHE_TRCKBONE_PASCUT & _pasCache, f32 index, f32 percent, bbool isMain, const AnimFrameInfo::BonesArray* _templateUsed, bbool _minimalBones);
    #endif
#else
    #ifndef NO_RUNTIME_LOCK
        bbool                           fillTrackAtIndex(AnimTrackBone & track,  const AnimTrackBonesList & animBoneLst, f32 index, AnimLock *lock );
        void                            ComputeAnimFrame(SafeArray<AnimBoneDyn> *_dyn, ITF_VECTOR<AnimLock> *_lock, f32 index, f32 percent, bbool isMain, const AnimFrameInfo::BonesArray* _templateUsed, bbool _minimalBones);
    #else
        bbool                           fillTrackAtIndex(AnimTrackBone & track,  const AnimTrackBonesList & animBoneLst, f32 index );
        void                            ComputeAnimFrame(SafeArray<AnimBoneDyn> *_dyn, f32 index, f32 percent, bbool isMain, const AnimFrameInfo::BonesArray* _templateUsed, bbool _minimalBones);
    #endif
#endif
//#ifdef ITF_SUPPORT_COOKING
        void                            ComputeAnimFrameCook(VectorAnim<VectorAnimTrackBoneWithLock> _trackList, SafeArray<AnimBoneDynWithLock> *_dyn, ITF_VECTOR<AnimLock> *_lock, f32 index, f32 percent, bbool isMain, const AnimFrameInfo::BonesArray* _templateUsed = NULL);
//#endif // ITF_SUPPORT_COOKING

        ////// bml processing
        AnimTrackBML *                  GetAnimBMLFrame(float index);

        ////// bml processing
        AnimTrackPolyline *             GetAnimPolylineFrame(float index);

        ////// event processing
        i32                             GetEvents(f32 prevIndex, f32 index, AnimEventContainer &eventList, bbool _getAll = bfalse );
        f32                             GetEventFrame(StringID::StringIdValueType _crc, const StringID & _eventName ) const;

        f32                             getDuration();
        AnimSkeleton *                  getSkeleton();
        ResourceID                      getTexture(u32 _index);
        AnimPatchBank *                 getPatchBank(u32 _index);
        
        AnimTrackBoneWithLock           getTrackBoneFromDyn(SafeArray<AnimBoneDynWithLock> & dyn, int _index, f32 _frame);
        
        /////// cook processing
        void                            removeLock(VectorAnim<VectorAnimTrackBoneWithLock> & _tempTrackList, bbool useDefaultImages = bfalse);
        void                            useMagicBoxAsRoot(VectorAnim<VectorAnimTrackBoneWithLock> &_tempTrackList);
        void                            computeMaxValues(const VectorAnim<VectorAnimTrackBoneWithLock> & _tempTrackList);
        void                            fillPASZALListFromBoneList(const VectorAnim<VectorAnimTrackBoneWithLock> & _tempTrackList, VectorAnim<VectorAnimTrackBonePAS> & pasList, VectorAnim<VectorAnimTrackBoneZAL> & zalList );
        bbool                           searchSubAnims(VectorAnim<SubAnimBeginEnd> &subAnims);
        bbool                           fillSoundFromEvents();
        void                            cleanPASKeys(VectorAnim<VectorAnimTrackBonePAS> & pasList);
        void                            cleanZALKeys(VectorAnim<VectorAnimTrackBoneZAL> & zalList);
        void                            fillPASZAL(const VectorAnim<VectorAnimTrackBonePAS> & pasList, const VectorAnim<VectorAnimTrackBoneZAL> & zalList);

        typedef bbool (*cookAnimationTrackCB)(const String&,const String&, ResourceGroup *_resGroup, bbool);
        typedef bbool (*cookAnimationTrackForcedCB)(const String&,const String&, ResourceGroup *_resGroup, bbool, bbool);

        virtual bbool                   needRecook();
        virtual void                    clear();

        static cookFileCallback         m_funcCookerTrack;

        AnimTrack();
        ~AnimTrack();
     };
    VECTAGSET(AnimTrack, ITF::MemoryId::mId_VectorAnimation)

    class BankRedirect
    {
    public:
        ITF_VECTOR<ResourceID>          m_textureResourceIDList;
        ITF_VECTOR<ResourceID>          m_patchBankResourceIDList;
        NamesMap                        m_nameToPatch;

        bbool           getTemplate(const TemplateRef &_ref, AnimTemplate * &_template) const;
        bbool           getTexture(const TemplateRef &_ref, ResourceID &_texture) const;


    private:
        ResourceID      getExtTexture(u32 _index) const;
        AnimPatchBank*  getExtPatchBank(u32 _index) const;
    };
    VECTAGSET(BankRedirect, ITF::MemoryId::mId_VectorAnimation)

    class AnimTrackExt
    {
    public:
        AnimTrackExt();

        ResourceID                      m_trackResourceID;
        
        KeyArray<u32>                   m_stateToRedirect;
        ITF_VECTOR<BankRedirect>        m_redirectStates;
        
        AnimTrack *     getTrack() const;
        AnimSkeleton *  getSkeleton() const;

        bbool           getTemplate(const TemplateRef &_ref, AnimTemplate * &_template, u32 _state) const;
        bbool           getTexture(const TemplateRef &_ref, ResourceID &_texture, u32 _state) const;
    };
    VECTAGSET(AnimTrackExt, ITF::MemoryId::mId_VectorAnimation)


    class PbkRedirect : public AnimationSerialize
    {
    public:
        PbkRedirect() {}

        void                        serialize(ArchiveMemory& _Archive, bbool _compressedData=bfalse);
        virtual void                clear();
        bbool                       findPatchBank(const String & _currentPath, String &_path);

    public:
        ITF_VECTOR<String>              m_patchBankPathList;
        KeyArray<i32>                   m_oldPatchBankToNew;
    };

    ITF_INLINE AnimTrack * AnimInfo::getFrameTrackReal(i32 _index) const
    {
        const AnimTrackExt * trkModif = getFrameTrackExt(_index);
        if (!trkModif)
            return NULL;
        else
            return trkModif->getTrack();
    }

    ITF_INLINE AnimSkeleton * AnimInfo::getFrameSkeleton(i32 _index) const
    {
        AnimTrack * trk = getFrameTrackReal(_index);
        if (!trk)
            return NULL;
        else
            return trk->getSkeleton();
    }

    ITF_INLINE const AnimTrackExt * AnimInfo::getFrameTrackExt(i32 _index) const
    {
        ITF_ASSERT_CRASH(_index < (i32)m_frameInfo.size(), "Invalid index, forcing crash to prevent data corruption");
        return m_frameInfo[_index].m_pTrackExt;
    }

    ITF_INLINE AnimSkeleton * AnimInfo::getCurrentSkeleton(i32 _index)  { 
        return getCurrentTrackReal(_index)->getSkeleton();
    }

    ITF_INLINE AnimSkeleton * AnimInfo::getWorkingSkeleton(i32 _index)  {
        return getWorkingTrackReal(_index)->getSkeleton(); 
    }

    ITF_INLINE AnimTrack * AnimInfo::getCurrentTrackReal(i32 _index)    {
        return getCurrentTrackExt(_index)->getTrack(); 
    }

    ITF_INLINE AnimTrack * AnimInfo::getWorkingTrackReal(i32 _index)    { 
        return getWorkingTrackExt(_index)->getTrack(); 
    }
}

#endif // _ITF_ANIMATIONTRACK_H_
