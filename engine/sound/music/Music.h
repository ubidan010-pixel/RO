#ifndef _ITF_MUSIC_H_
#define _ITF_MUSIC_H_


#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_

#ifndef _ITF_MUSICTREE_H_
#include "engine/blendTree/MusicTree/MusicTree.h"
#endif //_ITF_MUSICTREE_H_

#ifndef _ITF_SOUND_ADAPTER_
#include "engine/AdaptersInterfaces/SoundAdapter.h"
#endif //_ITF_SOUND_ADAPTER_


// This object is basically a container for music for a map or a situation 
// the data referenced in the tree must exist in the MusicPart template

namespace ITF
{
    class MusicPartSet_Template;
    class MusicPartData;
    class MusicInstance;

    ///////////////////////////////////////////////////////////////////////
    // MUSIC
    ///////////////////////////////////////////////////////////////////////

    class Music
    {
    public:

        Music();
        virtual ~Music();

        void                                init( const MusicPartSet_Template * _setTemplate, const MusicTree_Template * _treeTemplate, const SafeArray <InputDesc>& _inputList );

        ResourceGroup*                      getResourceGroup();

        virtual const MusicPart*            getMusicPart( const StringID _name ) const;

        const MusicPart*                    getFirstMusicPart() const;

        const MusicTree_Template*           getMusicTreeTemplate() const { return m_musicTreeTemplate; }

        // Inputs
        const SafeArray<InputDesc> *        getInputList() const { return m_inputList;}

        const u32                           getMetronomeType() const { return m_metronomeType; }

        bbool                               isPlaying() const;

        bbool                               isPhysicallyLoaded() const;

        ITF_INLINE void                     setMusicInstance( MusicInstance* _instance ) { m_musicInstance = _instance; }

        ITF_INLINE MusicInstance*           getMusicInstance() { return m_musicInstance ; }


    protected:

        ResourceID				            m_resourceGroup;

        ITF_VECTOR<ResourceID>              m_musicPartResourceIDList;

    private:

        const SafeArray <InputDesc> *       m_inputList;
        //InputContainer                    m_inputs;

        const MusicTree_Template*           m_musicTreeTemplate;

        // temp...

        MusicInstance*                      m_musicInstance;

        // music type
        u32                                 m_metronomeType;

    };

    ///////////////////////////////////////////////////////////////////////
    // MUSICSIMPLE
    ///////////////////////////////////////////////////////////////////////

    class MusicSimple : public Music
    {
    public:

        MusicSimple();
        ~MusicSimple(){};

        void                                init( const StringID& _name, const Path& _resourcePath, const StringID& _eventId);

    private:
    };    

}

#endif //_ITF_MUSIC_H_
