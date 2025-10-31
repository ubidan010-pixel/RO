#ifndef ITF_ROUTINGGROUP_H
#define ITF_ROUTINGGROUP_H



// during each frame:

// 1) AI calls whatever setDecibelVolume on whatever sounds (RoutingChild) and whatever groups 
// when requesting new volume on a group, m_voiceNeedVolumeUpdate is set to true
// when requesting new volume on a voice, new volume is stored and m_voiceNeedVolumeUpdate is set to true
//    -> volumes are not passed to low level 

// at the end of the frame:

// 2) updateFade() on each RoutingGroup to update internal volume 

// 3) updateVolume() must be called on each RoutingChild, which checks the m_voiceNeedVolumeUpdate flag on each voice 
//    and the voicesNeedVolumeUpdate() method on each parent to know if low level volume must be set 

// 4) resetVolumeUpdateFlag() must be called on each group to set m_voicesNeedVolumeUpdate back to false 

// on platforms where the group is a bus, the bus volume is set and m_voicesNeedVolumeUpdate is not set to true (hope we get to this point)


namespace ITF
{



    class RoutingGroup
    {
    public:

        RoutingGroup();

        virtual ~RoutingGroup();

        virtual void internalSetDecibelVolume( f32 _decibelVolume ) = 0;

        void updateFade( f32 _deltaTime );

        void setDecibelVolume( f32 _decibelVolume, f32 _fadeDurationSeconds = 0.f );

        virtual void resetVolumeUpdateFlag() = 0;

        virtual f32 getDecibelVolume() = 0;

        virtual bool childrenNeedVolumeUpdate() = 0;

        inline void setParentGroup( RoutingGroup * _parentGroup );

        inline RoutingGroup * getParentGroup();

    private:

        RoutingGroup * m_parentGroup;

        f32 m_fadeDuration;

        f32 m_currentFadeTime;

        f32 m_currentDecibelVolume;

        f32 m_fadeBeginningDecibelVolume;

        f32 m_fadeEndDecibelVolume;

        bool m_fading;
    };



    void RoutingGroup::setParentGroup( RoutingGroup * _parentGroup )
    {
        m_parentGroup = _parentGroup;
    }


    RoutingGroup * RoutingGroup::getParentGroup()
    {
        return m_parentGroup;
    }



} 




#endif


