#ifndef _ITF_MUSICPART_H_
#define _ITF_MUSICPART_H_



#ifndef _ITF_MUSICPARTTEMPLATE_H_
#include "engine/sound/Music/MusicPartTemplate.h"
#endif // _ITF_MUSICPARTTEMPLATE_H_



namespace raki
{
    class MemoryBlock;
    class WaveFile;
}


namespace ITF
{

    class MusicPart
        : public Resource
    {
    public:


        MusicPart( const Path & _path );
        ~MusicPart();

        virtual bbool                           tryLoadFile();
        virtual void                            flushPhysicalData();
        virtual ResourceType                    getType()const;
#ifdef ITF_SUPPORT_COOKING
        virtual bbool mustBeCooked()            const   {return btrue;}
#endif //ITF_SUPPORT_COOKING
#ifdef ITF_SUPPORT_RAKI

        ITF_INLINE const raki::MemoryBlock *    getPrefetchData() const                     { return m_prefetchData; }
#endif
        ITF_INLINE void                         setTemplate( const MusicPart_Template & _template );

#ifdef ITF_WII
        ITF_INLINE void                         forcePrefecth(bbool _forcePrefetch) { m_forcePrefetch = _forcePrefetch; }
#endif

        ITF_INLINE const StringID&              getName() const;
        ITF_INLINE void                         setName(const StringID& _name) { m_name = _name;}
/*
        ITF_INLINE const StringID&              getName() const                             { return m_name; }
        ITF_INLINE const Path&                  getPath() const                             { return m_path; }
        ITF_INLINE const bbool                  getPrefetch() const                         { return m_prefetch; }
*/
        ITF_INLINE const u32                    getSizeInBars() const                       { return m_template.getNbMeasures(); }
        ITF_INLINE const u32                    getBeatsPerBar() const                      { return m_template.getBeatsPerBar(); }
#ifdef ITF_SUPPORT_RAKI
        ITF_INLINE const u32                    getSampleSize() const                       { return m_sampleSize ; } 
        ITF_INLINE const u32                    getByteSize() const                         { return m_byteSize ; } 
        ITF_INLINE const u32                    getBytesPerSample() const                   { return m_bytesPerSample; } // this only has sense for PCM
#else
        ITF_INLINE const f32                    getWaveDuration() const                     { return m_waveDuration; }
        ITF_INLINE const void                   setWaveDuration(f32 _waveDuration)  { m_waveDuration = _waveDuration; }
        ITF_INLINE const u32                    getSampleSize() const { return m_sampleSize; }
        ITF_INLINE  void                        setSampleSize(const u32 _sampleSize) {
            m_sampleSize = _sampleSize;
        }



#endif

        ITF_INLINE const raki::WaveFile*        getWaveFile() const                         { return m_waveFile; }

        ITF_INLINE const StringID&              getPartName() const                         { return m_template.getName(); }

        ITF_INLINE  const StringID&             getWwiseEventGuid() const                   { return m_wwiseEventGuid; }
        ITF_INLINE  void                        setWwiseEventGuid(const StringID& _wwiseEventGuid);
  


    private:

        raki::MemoryBlock *                     m_prefetchData;
        raki::WaveFile *                        m_waveFile;
        MusicPart_Template                      m_template;
        u32                                     m_sampleSize;

        StringID                                m_wwiseEventGuid;
#ifndef ITF_SUPPORT_RAKI
        f32                                     m_waveDuration = 0.f;
#else
        u32                                     m_byteSize = 0;
        u32                                     m_bytesPerSample = 0;
#endif

        bbool                                   m_bUseTemplate;
#ifdef ITF_WII        
        bbool                                   m_forcePrefetch;
#endif
        StringID                                m_name;

    };


 
    void MusicPart::setTemplate( const MusicPart_Template & _template )
    {
        m_template      = _template;
        m_bUseTemplate  = btrue;
        m_wwiseEventGuid = _template.getWwiseEventGuid();

    }

    const StringID& MusicPart::getName() const                             
    { 
        if ( m_bUseTemplate )
            return m_template.getName(); 
        
        return m_name;
    }


    void MusicPart::setWwiseEventGuid(const StringID& _wwiseEventGuid)
    {
        m_wwiseEventGuid = _wwiseEventGuid;
    }
}

#endif //_ITF_MUSICPART_H_
