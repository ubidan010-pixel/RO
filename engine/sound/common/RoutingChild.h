#ifndef ITF_ROUTINGCHILD_H
#define ITF_ROUTINGCHILD_H




#define RAKI_ROUTING_MIN_VOLUME_DECIBELS                ( -96.f )



namespace ITF
{


    class RoutingGroup;



    class RoutingChild
    {
    public:

        RoutingChild(); 

        virtual ~RoutingChild();

        void setDecibelVolume( f32 _decibelVolume );

        void setLinearVolume( f32 _linearVolume );

        virtual void internalSetDecibelVolume( f32 _decibelVolume ) = 0;

        virtual void updateVolume( bool _force = false );

        virtual void setRoutingGroup( RoutingGroup * _routingGroup );

        // could be in some kind of math.h instead...
        inline bool floatsAlmostEqual( const f32 _a, const f32 _b );

        inline f32 getDecibelVolume();

        inline f32 getLinearVolume();
        inline const f32 getLinearVolumeFromDecibelVolume(f32 _decibelVolume);
        inline const f32 getDecibelVolumeFromLinearVolume(f32 _linearVolume);

    private:

        RoutingGroup * m_routingGroup;

        f32 m_decibelVolume;

        bool m_needsVolumeUpdate;

    };


    bool RoutingChild::floatsAlmostEqual( const f32 _a, const f32 _b )
    {
        return ( fabsf( ( _a ) - ( _b ) ) ) < 1.e-5;
    }

    f32 RoutingChild::getDecibelVolume()
    {
        return m_decibelVolume;
    }

    const f32 RoutingChild::getDecibelVolumeFromLinearVolume(f32 _linearVolume)
    {
        static const f32 log10dividedBy20 = 0.1151292546497f;

        return logf(_linearVolume) / log10dividedBy20;
    }


    const f32 RoutingChild::getLinearVolumeFromDecibelVolume(f32 _decibelVolume)
    {
        static const f32 log10dividedBy20 = 0.1151292546497f;

        return expf(_decibelVolume * log10dividedBy20);
    }

    f32 RoutingChild::getLinearVolume()
    {
        if ( m_decibelVolume <= RAKI_ROUTING_MIN_VOLUME_DECIBELS )
            return 0.f;

        return getLinearVolumeFromDecibelVolume( m_decibelVolume );
    }





} 




#endif


