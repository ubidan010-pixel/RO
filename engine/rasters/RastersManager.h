#ifndef _ITF_RASTERSMANAGER_H_
#define _ITF_RASTERSMANAGER_H_

#ifndef _ITF_KEYARRAY_H_
#include "core/container/KeyArray.h"
#endif //_ITF_KEYARRAY_H_

#ifndef _ITF_COLOR_H_
#include "core/Color.h"
#endif //_ITF_COLOR_H_

#include "core/templateSingleton.h"

#if defined(RASTER_ENABLED)
//We do not protect this file with include guards
#include "engine/rasters/RasterGroups_Defines.h"

namespace ITF
{
    class renderTarget;

    	
    ///////////////////////////////////////////////////////////////////////////////////////////
    class RasterID
    {
        friend class RastersManager;
    public:
        RasterID(const char *_name, u32 _group, const Color &_color) : m_name(_name), m_group(_group), m_color(_color) 
            { m_CRC = StringID::StrToCRC(_name); }

        const char *getName() const {return m_name;}
        u32 getGroup() const {return m_group;}
        StringID::StringIdValueType getCRC() const {return m_CRC;}
        const Color &getColor() const {return m_color;}

        void changeName(const char *_name, const Color &_color) {m_name = _name; m_CRC = StringID::StrToCRC(_name); m_color = _color; }

    protected:
       
        const char *m_name;
        u32 m_group;
        Color m_color;
        StringID::StringIdValueType m_CRC;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    ///The raster manager
    class RastersManager:public TemplateSingleton<RastersManager>
    {
    private:
        class Raster
        {
        public:
            Raster(const char* _name, u32 _rasterGroups, const Color &_color) : m_name(_name), m_hit(0), m_startTime(0), m_elapsedTime(0), m_rasterGroups(_rasterGroups), m_color(_color)
            {}
            Raster() : m_name(NULL), m_hit(0), m_startTime(0), m_elapsedTime(0), m_rasterGroups(0)
            {}

            void init();
            void reset() {m_elapsedTime = 0; m_hit = 0;}
            void stop();
            void start();

            ITF_INLINE const char* getName() const {return m_name;}
            ITF_INLINE u32 getGroups() const {return m_rasterGroups;}
            ITF_INLINE const Color& getColor() const {return m_color;}
            ITF_INLINE f32 getElapsedTime() const;
            ITF_INLINE u32 getNumberHit() const {return m_hit;}

        private:
            const char*   m_name;
            Color   m_color;
#ifndef ITF_WII
            volatile f32 m_startTime, m_elapsedTime;
#else
            volatile OSTick m_startTime, m_elapsedTime;
#endif
            u32     m_hit;
            u32     m_rasterGroups;
        };

    public:

        RastersManager():m_bEnable(bfalse),m_bEnableHistory(bfalse), m_renderTarget(NULL), m_rasterGroupIndex(0), m_rasterGroupChanged(bfalse), m_referenceTime(1.f/60.f), m_referenceTimeChanged(bfalse), m_lastTimeCaptionChanged(0), m_currentCaptionChangeIndex(0), m_currentCaptionColor(0,0,0,0), m_lastTimeGroupChanged(0) {}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// draw registered rasters on screen
        void rastersDraw();
		void rastersMinimalDraw(f32 minVal = 0.f);
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// reset values of every raster, and add them into history (if available)
        void rastersReset();
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// record start time for a code section
        /// @param _name : Name of code section
        /// @param _group : raster group
        void timeRasterStart(const RasterID &_rasterID) volatile;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// stop/pause chronometer for a code section
        /// @param _rasterID
        void timeRasterStop(const RasterID &_rasterID) volatile;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// same as other overload, but uses crc of its name
        /// @param _CRC
        void timeRasterStop(StringID::StringIdValueType _CRC) volatile;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// set reference time
        /// @param _referenceTime the reference time. Eg if you pass 1/60.f, the bold bar in the raster view will figure a 60Hz frame
        void setReference(f32 _referenceTime) {m_referenceTimeChanged = (m_referenceTime!=_referenceTime);m_referenceTime = _referenceTime;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Flush a raster to raster history. This is only to use if you change a RasterID name : call this function after calling rasterStop
        void flushDynamicRaster(const RasterID &_rasterID, ObjectRef _ref = ObjectRef::InvalidRef);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// find a raster by code section name
        /// @param _id
        Raster* find(const RasterID &_id) volatile;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// enable/disable rasterStart/rasterStop and history
        /// @param _bEnable
        void setEnable(bbool _bEnable)  {m_bEnable = _bEnable;}
        
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// accessor to enabled state
        bbool isEnabled() const   {return m_bEnable;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// clear history (if available)
        void resetHistory();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// dump history
        void dumpHistory();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// dump
        void dump(bbool allGroups = btrue);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// enable History
        /// @param _bEnableHistory
        void setEnableHistory(bbool _bEnableHistory) {m_bEnableHistory = _bEnableHistory;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// accessor to enabled state for history
        bbool isHistoryEnabled() const   {return m_bEnableHistory;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// change current raster group by multiplying it by 2, and return to 1 if >= END
        void cycleRasterGroup();

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Utility function for holding a registry of color from a name
        static const Color &getColorFromName(const char *_name, SafeArray<StringID::StringIdValueType> &_colorRegistry);

    protected:
        class RasterHistory
        {
        public:
            static const int maxSampleCount=256;
            RasterHistory(): m_sampleCount(0), m_name(), m_group(0) {}

            void reset() {m_sampleCount = 0;}
            void addSample(f32 _sampleValue, u32 _frameIndex);
            void dump();
            void setColor(const Color &_c) {m_color = _c;}
            const Color& getColor() const {return m_color;}
            void setGroup(u32 _group) {m_group = _group;}
            u32 getGroup() const {return m_group;}
            u32 getSampleCount() {return m_sampleCount;}
            void getSampleByFrameIndex(u32 _frameIndex, i32 _maxBackSearch, f32 &_sampleValue);
            void getSample(u32 _sampleIndex, f32 &_sampleValue, u32 &_frameIndex) {_sampleValue = m_sampledValue[_sampleIndex]; _frameIndex = m_sampledFrameIndex[_sampleIndex];}

            String8 m_name;
            ObjectRef m_ObjRef;

        private:
            f32 m_sampledValue[maxSampleCount];
            u32 m_sampledFrameIndex[maxSampleCount];
            u32 m_sampleCount;
            u32 m_group;
            Color m_color;

        };

        void flushRaster(Raster &_raster, StringID::StringIdValueType _CRC, bbool _reset, ObjectRef _ref = ObjectRef::InvalidRef);
        static int rasterComparer(const void *p1, const void *p2);

        typedef KeyArray<Raster> RasterListType;
        RasterListType  m_rasterList;
        ITF_MAP<u32, RasterHistory> m_rasterHistoryList;

        renderTarget        *m_renderTarget;
        static const u32    m_renderTargetHeight = 100;
        bbool               m_bEnable;
        bbool               m_bEnableHistory;
        u32                 m_rasterGroupIndex;
        bbool               m_rasterGroupChanged;
        f32                 m_referenceTime;
        bbool               m_referenceTimeChanged;
        String              m_currentCaptionText;
        ITF_VECTOR<String>  m_rasterListText;
        f64                 m_lastTimeCaptionChanged;
        f64                 m_lastTimeGroupChanged;
        u32                 m_currentCaptionChangeIndex;
        Color               m_currentCaptionColor;

    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    ///Raster with automatic destruction at end of scope
    class timeRasterScope
    {
    public:
        timeRasterScope(const RasterID &_id);
        ~timeRasterScope();

    private:
        StringID::StringIdValueType m_CRC;
    };

} // namespace ITF

#define DECLARE_RASTER(_id, _rasterGroup, _color) static RasterID rasterID_##_id{#_id, _rasterGroup, _color}
#define RASTER_CHANGE_NAME(_id, _name, _color) rasterID_##_id.changeName(_name, _color)
#define FLUSH_DYNAMIC_RASTER(_id) RastersManager::get().flushDynamicRaster(rasterID_##_id)
#define FLUSH_DYNAMIC_RASTER_REF(_id,ref) RastersManager::get().flushDynamicRaster(rasterID_##_id,ref)
#define TIMERASTER_SCOPE(_id) const timeRasterScope __LINE__##Raster{rasterID_##_id}
#define TIMERASTER_START(_id) RastersManager::get().timeRasterStart(rasterID_##_id)
#define TIMERASTER_STOP(_id)  RastersManager::get().timeRasterStop(rasterID_##_id)
#define RASTER_DUMP(__allgroups)  RastersManager::get().dump(__allgroups)

#define M_RASTER_GET(_rasterManager) RastersManager& _rasterManager =  RastersManager::get()
#define M_RASTER_RESET(_rasterManager) _rasterManager.rastersReset()
#define M_RASTER_DRAW() RastersManager::get().rastersDraw()
#define RASTER_SETREFERENCE(_reference) RastersManager::get().setReference(_reference)
#define RASTER_CYCLERASTERGROUP() RastersManager::get().cycleRasterGroup()
#endif 

///////////////////////////////////////////////////


#if !defined(RASTER_ENABLED)
#define DECLARE_RASTER(_id, _rasterGroup, _color) 
#define RASTER_CHANGE_NAME(_id, _name, _color)
#define FLUSH_DYNAMIC_RASTER(_id)
#define TIMERASTER_SCOPE(_id)
#define TIMERASTER_START(_id)
#define TIMERASTER_STOP(_id)
#define M_RASTER_GET(_rasterManager)
#define M_RASTER_RESET(_rasterManager)
#define M_RASTER_DRAW()
#define RASTER_SETREFERENCE(_reference)
#define RASTER_DUMP(__allgroups)
#define RASTER_CYCLERASTERGROUP()

#endif


#endif //_ITF_RASTERSMANAGER_H_
