#ifndef _ITF_INPUTINFO_H_
#define _ITF_INPUTINFO_H_

namespace ITF
{

	struct SInputInfo
	{
		enum EInputType
		{
			INPUTTYPE_AXIS=0,
			INPUTTYPE_BUTTON,
            INPUTTYPE_ACCELEROMETER
		};

		enum EInputQuery
		{
			QUERY_AXIS=0,
			QUERY_PRESS,
			QUERY_RELEASE,
			QUERY_HOLD,
            QUERY_ACC,
            QUERY_UNSET,
			QUERY_MAX
		};
	    
		enum EButtonStatus
		{
			BUTTONSTATUS_NONE = 0,
			BUTTONSTATUS_PRESS,
			BUTTONSTATUS_HOLD,
		};

		struct SAxisInfo
		{
			float           m_axis;
			EButtonStatus   m_status;
		};
	    
		struct SButtonInfo
		{
			EButtonStatus   m_status;
		};

        struct AccInfo
        {
            f32 x, y, z;
        };

        struct SAccelerometerInfo
        {
            AccInfo     m_acc, m_prevAcc;
        };

		union {
			SAxisInfo           m_axisInfo;
			SButtonInfo         m_buttonInfo;
            SAccelerometerInfo  m_accelerometerInfo;
		};

		EInputType           m_type;
		f64                  m_pressTime;
		bbool                m_dirty;
	};

};

#endif // _ITF_INPUTINFO_H_