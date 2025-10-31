#ifndef _ITF_SUBANIMLIGHTSET_H_
#define _ITF_SUBANIMLIGHTSET_H_

#ifndef _ITF_ANIMATION_H_
#include    "engine/animation/Animation.h"
#endif // _ITF_ANIMATION_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include    "engine/animation/AnimationTrack.h"
#endif // _ITF_ANIMATIONTRACK_H_

#ifndef _ITF_METRONOMEMANAGER_H_
#include "engine/sound/MetronomeManager.h"
#endif //_ITF_METRONOMEMANAGER_H_

namespace ITF
{
    struct BankChangeResourceID
    {
        ResourceID  m_patchBankResourceID;
        ResourceID  m_textureResourceID;
    };

    class SubAnim_Template
    {
    public:

        DECLARE_SERIALIZE()

        SubAnim_Template();
        ~SubAnim_Template();

        ITF_INLINE const StringID&      getFriendlyName() const                     { return m_friendlyName; }
        ITF_INLINE const Path&          getAnimPath() const                         { return m_animPath; }
        ITF_INLINE const StringID&      getStartMarker() const                      { return m_markerStart; }
        ITF_INLINE const StringID&      getStopMarker() const                       { return m_markerStop; }
        ITF_INLINE f32                  getPlayRate() const                         { return m_playRate; }
        ITF_INLINE f32                  getSyncRatio() const                        { return m_syncRatio; }
        ITF_INLINE u32                  getSyncEigthNote() const                    { return m_syncEighthNote; }
        ITF_INLINE bbool                getUseRootRotation() const                  { return m_useRootRotation; }
        ITF_INLINE f32	                getShadowMul() const						{ return m_shadowMul; }
        ITF_INLINE bbool                getIsLooped() const                         { return m_loop; }
        ITF_INLINE bbool                getIsProcedural() const                     { return m_procedural || m_sync; }
        ITF_INLINE bbool                getDefaultFlip() const                      { return m_defaultFlip; }
        ITF_INLINE bbool                getIsSync() const                           { return m_sync; }
        ITF_INLINE MetronomeType        getMetronome() const                        { return m_metronomeType; }
        ITF_INLINE bbool                getIsReversed() const                       { return m_reversed; }
        ITF_INLINE bbool                isValid() const                             { return !m_animPath.isEmpty(); }
        ITF_INLINE bbool                allowSyncOffset() const                     { return m_allowSyncOffset; }

        ITF_INLINE f32                  getStart() const                            { return m_start; }
        ITF_INLINE f32                  getStop() const                             { return m_stop; }

        ITF_INLINE void                 setFriendlyName( const StringID& _name )    { m_friendlyName = _name; }
        ITF_INLINE void                 setAnimPath( const String& _name )          { m_animPath = _name; }
        ITF_INLINE void                 setUseRootRotation( bbool _val )            { m_useRootRotation = _val; }
        ITF_INLINE void                 setIsLooped( bbool _looped )                { m_loop = _looped; }
        ITF_INLINE void                 setSyncEigthNote(u32 _syncEightNote)        { m_syncEighthNote = _syncEightNote; }
        ITF_INLINE void                 setMetronome(MetronomeType _metronomeType)  { m_metronomeType = _metronomeType; }

        ITF_INLINE void                 setStart( f32 _val )                        { m_start = _val; }
        ITF_INLINE void                 setStop( f32 _val )                         { m_stop = _val; }

        ITF_INLINE void                 setTrack(ResourceID _trackResource);
        ITF_INLINE const AnimTrackExt*  getTrackExt() const                         { return &m_trackExt; }

        AnimTrack  *                    resolveMarkers(const String & _animPath);


    private:
        friend class SubAnimSet_Template;

        Path                            m_animPath;
        StringID                        m_friendlyName;
        StringID                        m_markerStart;
        StringID                        m_markerStop;
        f32                             m_playRate;
        f32                             m_syncRatio;
		f32								m_shadowMul;
        u32                             m_syncEighthNote;
        bbool                           m_loop;
        bbool                           m_useRootRotation;
        bbool                           m_reversed;
        bbool                           m_defaultFlip;
        bbool                           m_procedural;
        bbool                           m_sync;
        bbool                           m_allowSyncOffset;
        MetronomeType                   m_metronomeType;

        // computed data ...
        AnimTrackExt                    m_trackExt;
        f32                             m_start;
        f32                             m_stop;
    };

    class SubAnim
    {
    public:

        SubAnim()
        : m_template(NULL)
        , m_templateInternal(NULL)
        , m_syncOffset(0.f)
        , m_randomStart(bfalse)
        {
        }

        ~SubAnim() { SF_DEL(m_templateInternal); }

        ITF_INLINE const AnimTrackExt*  getTrackExt() const                         { return m_template->getTrackExt(); }
        ITF_INLINE AnimTrack *          getTrack()  const                           { return getTrackExt()->getTrack(); }
        ITF_INLINE AnimSkeleton *       getSkeleton() const                         { return getTrack()->getSkeleton(); }
        ITF_INLINE const StringID&      getFriendlyName() const                     { return m_template->getFriendlyName(); }
        ITF_INLINE const Path&          getAnimPath() const                         { return m_template->getAnimPath(); }
        ITF_INLINE const StringID&      getStartMarker() const                      { return m_template->getStartMarker(); }
        ITF_INLINE const StringID&      getStopMarker() const                       { return m_template->getStopMarker(); }
        ITF_INLINE f32                  getDuration() const                         { return m_template->getStop() - m_template->getStart();}
        ITF_INLINE f32                  getPlayRate() const                         { return m_template->getPlayRate(); }
        ITF_INLINE f32                  getSyncRatio() const                        { return m_template->getSyncRatio(); }
        ITF_INLINE u32                  getSyncEigthNote() const                    { return m_template->getSyncEigthNote(); }  // [0,8] range
        ITF_INLINE f32                  getSyncOffset() const                       { return m_syncOffset; }                    // [0,1] range (normalized)
        ITF_INLINE bbool                getUseRootRotation() const                  { return m_template->getUseRootRotation(); }
        ITF_INLINE bbool                getUseRandomStart() const                   { return m_randomStart; }
        ITF_INLINE bbool                getIsLooped() const                         { return m_template->getIsLooped(); }
        ITF_INLINE bbool                getIsProcedural() const                     { return m_template->getIsProcedural(); }
        ITF_INLINE bbool                getDefaultFlip() const                      { return m_template->getDefaultFlip(); }
        ITF_INLINE bbool                getIsSync() const                           { return m_template->getIsSync(); }
        ITF_INLINE MetronomeType        getMetronome() const                        { return m_template->getMetronome(); }
        ITF_INLINE bbool                getIsReversed() const                       { return m_template->getIsReversed(); }
		ITF_INLINE f32					getShadowMul() const						{ return m_template->getShadowMul(); }
        bbool                           isValid() const                             { return m_template && m_template->isValid(); }
        bbool                           allowSyncOffset() const                     { return m_template && m_template->allowSyncOffset(); }
        ITF_INLINE f32                  getStart() const                            { return m_template->getStart(); }
        ITF_INLINE f32                  getStop() const                             { return m_template->getStop(); }

        void                            setSyncOffset( f32 _val );
        void							setTemplate( const SubAnim_Template* _template );
        ITF_INLINE void                 setInternalTemplate( const SubAnim_Template* _template ) { setTemplate(_template); m_templateInternal = _template; }
        ITF_INLINE const SubAnim_Template*  getTemplate() const                     { return m_template; }

        void                            invalidate() { m_template = NULL; }

    private:

        const SubAnim_Template*         m_template;
        const SubAnim_Template*         m_templateInternal;

        f32                             m_syncOffset;   // normalized; global offset set by the AnimLightComponent
        bbool                           m_randomStart;
    };

    class BankChange_Template
    {
        DECLARE_SERIALIZE()

    public:

        BankChange_Template();
        ~BankChange_Template() {}

        const StringID&             getBankName() const { return m_bankName; }

        const Path&                 getBankPath() const { return m_bankPath; }
        void                        setBankPath(const Path& _path) { m_bankPath = _path; }

        const StringID&             getFriendlyName() const { return m_friendlyName; }
        u32                         getState() const { return m_state; }

    private:

        StringID    m_bankName;
        Path        m_bankPath;
        StringID    m_friendlyName;
        u32         m_state;
    };

    struct BankChange
    {
        DECLARE_SERIALIZE()

        ResourceID  m_patchBankResourceID;
        ResourceID  m_textureResourceID;

        BankChange() : m_template(NULL) { }

        const BankChange_Template*  m_template;
    };

    class SubAnimSet_Template
    {
        DECLARE_SERIALIZE()

    public:

        SubAnimSet_Template();
        ~SubAnimSet_Template() {}

        void                                    postSerialize(const String & _path);

        const ITF_VECTOR<SubAnim_Template>&     getSubAnimList() const { return m_subAnimList; }
        const ITF_VECTOR<BankChange_Template>&  getBankChangeList() const { return m_bankChangeList; }
        const ITF_VECTOR<String8>&              getSkipFiles() const { return m_skipFiles; }
        const NamesMap&                         getNameToSubAnim() const { return m_nameToSubAnim; }
        const ITF_VECTOR<Path>&                 getResourceList() const { return m_resourceList; }
        const ITF_VECTOR<u32>&                  getResourceTypeList() const { return m_resourceTypeList; }
        const AABB &                            getAnimAABB() const { return m_animsAABB; }
        const SafeArray<u32>&                   getNameIdList() const { return m_nameId; }
        const SafeArray<i32>&                   getNameResIdxList() const { return m_nameResIdx; }


        void                                    addBankInfoToSubAnimTemplate(SubAnim_Template *_subAnimTemplate, AnimTrack * track, const ITF_VECTOR<BankChangeResourceID> & _bankResources);
        bbool                                   resolveMarkers(const String & _animPath);
        bbool                                   markerResolded() const { return m_markerResolved; }

    private:

        ITF_VECTOR<SubAnim_Template>            m_subAnimList;
        ITF_VECTOR<BankChange_Template>         m_bankChangeList;
        ITF_VECTOR<String8>                     m_skipFiles;
        ITF_VECTOR<Path>                        m_resourceList;
        ITF_VECTOR<u32>                         m_resourceTypeList;
        AABB                                    m_animsAABB;
        NamesMap                                m_nameToSubAnim;
        SafeArray<u32>                          m_nameId;
        SafeArray<i32>                          m_nameResIdx;


        // computed data
        bbool                                   m_markerResolved;
        bbool                                   m_redirectDone;
    };

    class SubAnimSet : public TrackSet // inherit from Track to simplify use
    {
    public:

        SubAnimSet();
        ~SubAnimSet();

        u32                         getSubAnimCount() const { return m_subAnimList.size(); }
        SubAnim *                   getSubAnim(i32 _index) const
        { 
            ITF_ASSERT(_index < (i32)m_subAnimList.size());
            if (_index < (i32)m_subAnimList.size())
                return m_subAnimList[_index]; 
            else 
                return NULL;  
        }
        SubAnim *                   getSubAnim(const StringID &_subAnimName) const;
        i32                         getSubAnimIndex(const StringID &_subAnimName) const;
        void                        addSubAnimList(SubAnim* _subAnim, const String & path);

        // Changing time can cast event -> need eventList & param
        void                        updateAnimTime(f32 _deltaTime, AnimEventContainer* _eventList);

        f32                         getAnimDuration(StringID _animName, bbool _full) const;
        f32                         getTrackDuration(const String &_path) const;

        void                        linkSubAnimToTrack(ResourceGroup *_resourceGroup, const Path& path); // used on serialization, after collecting animList data
        bbool                       resolveMarkers(const String & _path);

        void                        clear();
        bbool                       isResolveMarkersOk();

        void                        setTemplate( const class SubAnimSet_Template* _template );
        const SubAnimSet_Template*  getTemplate( ) const { return m_template; }

        void                        setGlobalSyncOffset( const f32 _syncOffset );

    protected:
        void                        computeSubAnimAtPos(int _pos, const String & path, const Path & _dependencyFile);

        NamesMap                    m_nameToSubAnim;
        const SubAnimSet_Template*  m_template;
        ITF_VECTOR<SubAnim*>        m_subAnimList;
        ITF_VECTOR<BankChange>      m_bankChangeList;
        SafeArray<u32>              m_addedSubAnim;

        f32                         m_syncOffset;
        bbool                       m_addedSubAnimMarkerResolved;
    };
}

#endif //_ITF_SUBANIMLIGHTSET_H_
