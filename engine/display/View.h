#ifndef _ITF_VIEW_H_
#define _ITF_VIEW_H_

#ifndef _ITF_BV_AABB_H_
#include "core/boundingvolume/AABB.h"
#endif //_ITF_BV_AABB_H_

namespace ITF
{

// #define USE_SPLITSCREEN

class Camera;

#define CAMERA  View::getMainView()->getCamera()

class View
{
public:
    enum ViewType
    {
        viewType_Camera,
        viewType_AABB,
        viewType_Count,
        ENUM_FORCE_SIZE_32(0)
    };

    //////////////////////////////////////////////////////////////////////////
    ///
    /// @return the view's type
    ViewType getViewType()const { return m_type; }

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Creates the camera for a view
    /// A view doesn't have a camera by default because some views can be created just to prefectch data loading for example
    /// This method will erase the existing camera if any
     void    createCamera();

    //////////////////////////////////////////////////////////////////////////
    ///
    /// @return the view's camera if any
     Camera* getCamera() {return m_camera;}

     //////////////////////////////////////////////////////////////////////////
     ///
     /// Set the view's target AABB (projected at Z = 0)
     void    setTargetAABB(const AABB& _aabb);

     //////////////////////////////////////////////////////////////////////////
     ///
     /// @return the view's target AABB (projected at Z = 0)
     const AABB&    getTargetAABB()const { return m_aabbAtZero; }

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Deletes the camera for a view
    /// Does nothing if the view has no camera
     void    deleteCamera();

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Defines the view as the main view
     void    setMainView();

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Assigns an objectRef to a view
     void    setLinkedObject(ObjectRef _or) {m_or = _or;}

    //////////////////////////////////////////////////////////////////////////
    ///
    /// @returns the objref assigned to the view
     ObjectRef getLinkedObject(ObjectRef _or) {return m_or;}

     //////////////////////////////////////////////////////////////////////////
    ///
    /// @return the main view
     static View*  getMainView() {return m_mainView;}

    //////////////////////////////////////////////////////////////////////////
    ///
    /// @return btrue if the view is the main view
    bbool   isMainView();

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Activates / desactivates a View
    void setActive(bbool _active);

    //////////////////////////////////////////////////////////////////////////
    ///
    /// @return btrue if the view is active
    bbool isActive() {return m_isActive;}

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Deletes the view (and its camera if any)
     static void    deleteView(View* _view);

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Creates a new view
    /// @return the created view
     static View* createView(const char* _name, bbool _canBeMainView = btrue, bbool _prefetch = bfalse);

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Deletes all the views
     static void    deleteAll();

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Returns the all the views
     static const SafeArray<View*>&    getViews() {return m_views;}

     const String8& getName() {return m_name;}

     ITF_INLINE void    setPrefetchDataOnly (bbool _prefectch) {m_prefetchDataOnly = _prefectch;}
     ITF_INLINE bbool   getPrefetchDataOnly () {return m_prefetchDataOnly;}

     void lockAABB (bbool _lock) {m_lockAABB = _lock;}
     bbool isLocked() const {return m_lockAABB;}
     void updateScreenProjAABB(const AABB& _aabb) {m_lastScreenProj = _aabb;}
     const AABB& getScreenProjAABB() const {return m_lastScreenProj;}

     static void   lockAll();
     static void   unLockAll();
     static void   switchLockAll();

private:
    View();     // only createView() is public
    ~View();    // only deleteView() is public

    static  SafeArray<View*>    m_views;
    static  View*               m_mainView;
    Camera*                     m_camera;
    ObjectRef                   m_or;
    bbool                       m_isActive;
    bbool                       m_prefetchDataOnly;
    bbool                       m_lockAABB;

    AABB                        m_aabbAtZero, m_lastScreenProj;
    ViewType                    m_type;
    String8                     m_name;
};

} // namespace ITF



#endif //_ITF_VIEW_H_
