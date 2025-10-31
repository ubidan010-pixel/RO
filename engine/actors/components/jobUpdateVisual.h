#ifndef _ITF_JOBUPDATEVISUAL_H_
#define _ITF_JOBUPDATEVISUAL_H_


namespace ITF
{

class AnimLightComponent;

class JobUpdateVisualContext
{
public:
    JobUpdateVisualContext():m_pAnimLightComponent(NULL) {};
    JobUpdateVisualContext(AnimLightComponent*  _pAnimLightComponent):
    m_pAnimLightComponent(_pAnimLightComponent)
    {

    }

    AnimLightComponent*  m_pAnimLightComponent;//read-write
};


class JobUpdateVisual :public Job
{
public:
    JobUpdateVisual() {};

    JobUpdateVisual(JobUpdateVisualContext& _context):m_context(_context)
    {

    }


    void setContext(const JobUpdateVisualContext& _context) {m_context = _context;}

    static u32 _jobTag_UpdateVisual;
    virtual u32 getTag() {return _jobTag_UpdateVisual;}
    void execute(bbool _immediate);

protected:
    JobUpdateVisualContext m_context;

};



extern void mainJobUpdateVisuals(JobUpdateVisualContext* _context);
extern void mainJobUpdateVisuals(AnimLightComponent*  _pAnimLightComponent);
}


#endif //_ITF_JOBUPDATEVISUAL_H_