#ifndef _ITF_LEADERBOARDSERVICE_
#define _ITF_LEADERBOARDSERVICE_

namespace ITF
{
//------------------------------------------------------------------------------------------------------------------------------------------------------
class Leaderboard;
class LeaderboardProperty;

//------------------------------------------------------------------------------------------------------------------------------------------------------


enum LeaderboardPropertyEnum : i32
{
    eLeaderBoardString = 0,
    eLeaderboardU32    = 1,
    eLeaderboardDouble = 2,
};


typedef SafeArray<LeaderboardProperty*> LeaderboardPropertyArray;
//------------------------------------------------------------------------------------------------------------------------------------------------------

class LeaderboardContext
{
public:
    LeaderboardContext():m_pLeaderboard(NULL),m_bDone(bfalse) {};
    virtual  ~LeaderboardContext() {};
    virtual void update() {ITF_ASSERT(0);}

    void setLeaderboard(const Leaderboard* _pLeaderboard) {m_pLeaderboard = _pLeaderboard;}

    const Leaderboard* getLeaderboard() {return m_pLeaderboard;}

protected:

    const Leaderboard*    m_pLeaderboard;
    bbool                 m_bDone;


    //result
    SafeArray<LeaderboardPropertyArray*> m_ResultProperties;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------

struct LeaderboardRequestID
{
public:
    LeaderboardRequestID() {/*ITF_ASSERT(0);*/}
    LeaderboardRequestID( LeaderboardContext* _pContext,u32 _currentRequestId)
        :m_pContext(_pContext),m_currentRequestId(_currentRequestId)
    {
        ITF_UNUSED(m_currentRequestId);
    } 

    LeaderboardContext* getContext() {return m_pContext;}

private:
    u32 m_currentRequestId;

    //platform context..
    LeaderboardContext* m_pContext;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------

class LeaderboardTemplateProperty
{
public:
    LeaderboardTemplateProperty(const String& _name,u32 _alias,LeaderboardPropertyEnum _type):m_Name(_name),m_Alias(_alias),m_type(_type)
    {

    }

    u32 getAlias() const {return m_Alias;}

protected:
    String          m_Name; 
    u32             m_Alias;
    LeaderboardPropertyEnum     m_type; 
};

//------------------------------------------------------------------------------------------------------------------------------------------------------

typedef SafeArray<LeaderboardTemplateProperty*> LeaderboardTemplatePropertyArray;

class  Leaderboard
{
public:
    Leaderboard(u32 _id):m_id(_id) {};
    virtual ~Leaderboard();

    void addTemplateProperty(const String& _name,u32 _alias, LeaderboardPropertyEnum _enum);

    u32 getId() {return m_id;}

    LeaderboardTemplateProperty*  getTemplatePropertyByIndex(u32 _index) const {return m_PropertiesArray[_index];}

    const LeaderboardTemplatePropertyArray& getTemplatePropertyArray() const {return m_PropertiesArray;}

protected:

    LeaderboardTemplatePropertyArray m_PropertiesArray;
    u32 m_id;
};


typedef SafeArray<Leaderboard> LeaderboardArray;

//------------------------------------------------------------------------------------------------------------------------------------------------------

class LeaderboardProperty
{
public:

    LeaderboardProperty(const String& _name,u32 _value) 
    {
        m_value = (void*)new u32();
        m_type  = eLeaderboardU32;
        m_name  = _name;
        *((u32*) m_value) = _value;
    };

    LeaderboardProperty(const String& _name,const String& _value) 
    {
        m_value = (void*)new String(_value);
        m_type  = eLeaderBoardString;
        m_name  = _name;
        *((String*) m_value) = _value;
    };

    LeaderboardProperty(const String& _name,double _value) 
    {
        m_value = (void*)new double(_value);
        m_type  = eLeaderboardDouble;
        m_name  = _name;
        *((double*) m_value) = _value;
    };

/*
    //We use float as a double
    LeaderboardProperty(const String& _name, f32 _value):LeaderboardProperty(_name,(double) _value)
    {

    }
*/   

    virtual  ~LeaderboardProperty() 
    {
       switch (m_type)
       {
           case eLeaderBoardString:
                delete ((String*)(m_value));
               break;
           case eLeaderboardU32:
                delete ((u32*)(m_value));
               break;
           case eLeaderboardDouble:
               delete ((double*)(m_value));
               break;
       }

    };

protected:
    LeaderboardPropertyEnum     m_type;
    void*                       m_value;
    String                      m_name;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------

class LeaderboardService
{
public:
    LeaderboardService():m_incrementRequestId(0),m_incrementLeaderboardId(0) {};
    virtual ~LeaderboardService();

    LeaderboardRequestID  writeDetails(const Leaderboard& _leaderboard,const LeaderboardArray& _properties);
    LeaderboardRequestID  getDetails(const Leaderboard& _leaderboard);
    LeaderboardRequestID  getMyRank(const Leaderboard& _leaderboard);
    LeaderboardRequestID  getInfosByRange(const Leaderboard& _leaderboard,u32 _startRange,u32 _endRange);

    LeaderboardRequestID getNewRequestID(const Leaderboard& _pLeaderboard);
    

    void destroy();
    void init();
    void update();

    bbool poll(const LeaderboardRequestID& _requestID);

protected:
    virtual void initInternal() {ITF_ASSERT(0);}

private:

    //move to pure function as soon as the system is working 
    virtual void requestRangeDetails(const LeaderboardRequestID& _leaderboardRequestID,u32 _startrange,u32 _endrange) {ITF_ASSERT(0);}
    virtual LeaderboardContext* createContext(const Leaderboard* _pLeaderBoard)                               {ITF_ASSERT(0);return NULL;}

    bool  writeDetailsTarget(const Leaderboard& _leaderboard,const LeaderboardArray& _properties) {ITF_ASSERT(0);return bfalse;}

    SafeArray<LeaderboardRequestID>     m_workingRequestId;
    SafeArray<Leaderboard*>             m_Leaderboards;

    u32 m_incrementRequestId;
    u32 m_incrementLeaderboardId;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------

} // namespace ITF



#endif //_ITF_LEADERBOARDSERVICE_
