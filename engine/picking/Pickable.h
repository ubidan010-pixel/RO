#ifndef _ITF_PICKABLE_H_
#define _ITF_PICKABLE_H_

#ifndef _ITF_BASEOBJECT_H_
#include "core/baseObject.h"
#endif //_ITF_BASEOBJECT_H_

#ifndef _ITF_BV_AABB_H_
#include "core/boundingvolume/AABB.h"
#endif //_ITF_BV_AABB_H_

namespace ITF
{

class   World;
class   Camera;
struct  PickingInfo2D;
class   ResourceGroup;
class   Scene;
class   Matrix44;
class   ObjectGroup;

enum PickableEditorChangeType
{
    ChangeType_Deleted,
    ChangeType_Reloaded,
    ChangeType_Changed
};

// #define TRACK_PICKABLE_FRIENDLY_NAME "ice_rail_1x1@1314291037144"

/**
* This struct is filled by preChangeProperties() and tested by postChangeProperties().
* It's used to check which fields have been changed by the properties editor.
*/
struct PickablePropertyEditState
{
    /** the object that is described by the structure */
    ObjectRef               m_object;

    /** the initial pos before changing properties */
    Vec3d                   m_previousInitPos;

    /** the initial angle before changing properties */
    f32                     m_previousInitAngle;

    /** the initial scale before changing properties */
    Vec2d                   m_previousInitialScale;

    /** the position before changing properties */
    Vec3d                   m_previousPos;

    /** the angle before changing properties */
    f32                     m_previousAngle;

    /** the scale before changing properties */
    Vec2d                   m_previousScale;
};

/**
* This class is the base class for most of the objects<br>
* <br>You'll find below a list of basic mechanisms used by this class<br>
* <br><a name="objfam"><b>Object Family:</b></a><br>
*  Objects can be assigned families. This is a bitfield that is used to filter edition display and picking features. For example, the user may want to see only gameplay element, thus hiding pure background visual objects.
* <br><a name="initproperties"><b>Initial Properties:</b></a><br>
*  Objects have a pos, rot and scale as well as an initial pos, initial rot and initial scale. Those initial values are used to restore positions when resetting the game (F5 key) while editing and also when reswpawning at checkpoints
* <br><a name="forcedattributes"><b>Forced Attributes:</b></a><br>
*  Template data can force attributes (Z, scale, rank...). Forced attributes always override instance data
* <br><a name="depthexplained"><b>Depth:</b></a><br>
*  The object's Depth is its Z coordinate. Depth is stored within the m_z member of its m_pos Vec3d.  
*/
class Pickable : public BaseObject
{
    DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(Pickable,BaseObject,594660361)
	DECLARE_SERIALIZE()

public:

    enum UpdateType
    {
        UpdateType_OnlyIfVisible = 0,
        UpdateType_OffscreenAllowed = 1
    };

    struct CellUpdateData
    {
        AABB    m_aabb;
        u32     m_layerIndex;
    };
    struct CellEnableData
    {
        CellEnableData()
        {
            m_isStatic = bfalse;
            m_isSubSceneActor = 0;
            m_updateType = UpdateType_OnlyIfVisible;
            m_mustBeInitializedWithCell = 0;
            m_hasEverBeenRegisteredInCells = 0;
        }
        AABB    m_aabb;
        u32     m_frame;
        u32     m_isStatic : 1,
                m_updateType : 2,
                m_mustBeInitializedWithCell : 1,
                m_hasEverBeenRegisteredInCells : 1,
                m_isSubSceneActor : 1;
    };

    CellEnableData&      getCellEnableData() {return m_lastCellEnableData;}

private:
    /*************** for better caching ,keep this section together*****/
    u32                     m_lastActiveFrame;
    u32                     m_lastVisibleFrame;
    u32                     m_active : 1,
                            m_isSelected : 1,
                            m_physicalReady : 1,
                            m_isAlwaysVisible : 1,
                            m_activeInSceneWhen2D : 1,
                            m_bHasDataError : 1,
                            m_taggedForDeletion : 1,
                            m_updateTypeSetByPropagation : 1;

    bbool                   m_isAllowedForCell;
    bbool                   m_isEnabled;
    bbool                   m_updateTypeFromTemplateOverridden;
    UpdateType              m_updateType;

    CellUpdateData      m_lastCellUpdateData;
    CellEnableData      m_lastCellEnableData;
    /***********************************************************************/

    ///////////////////////////////////////////////////////////////////////////
    /** Global section */
    ///////////////////////////////////////////////////////////////////////////
public:


    /** <a href="#objfam">see Object Family explanation</a> */
    static const u32   ObjectFamily_NotSet              =   0;
    /** <a href="#objfam">see Object Family explanation</a> */
    static const u32   ObjectFamily_LevelDesign         =   1;
    /** <a href="#objfam">see Object Family explanation</a> */
    static const u32   ObjectFamily_Art                 =   2;

	/** Starting from 1<<16, these are not filters, but hints */
    /** <a href="#objfam">see Object Family explanation</a> */
    static const u32   ObjectFamily_Alpha               =   1<<16;

                                Pickable();
    virtual                     ~Pickable();

    virtual void                setTaggedForDeletion();
    ITF_INLINE bbool            isTaggedForDeletion() {return m_taggedForDeletion;}

    ITF_INLINE bbool            isUpdateTypeFromTemplateOverridden()const { return m_updateTypeFromTemplateOverridden; }

    virtual void                setUpdateType(UpdateType _t, bbool _propagate = bfalse);
    ITF_INLINE UpdateType       getUpdateType()const { return m_updateType; }

    ///////////////////////////////////////////////////////////////////////////
    /** End of Global section */
    ///////////////////////////////////////////////////////////////////////////





    ///////////////////////////////////////////////////////////////////////////
    /** Postition section */
    ///////////////////////////////////////////////////////////////////////////
public:

    /** 
    * Casts the Vec3d position into Vec2d 
    * @return const reference to the casted Vec3d member
    */
    ITF_INLINE  const Vec2d&    get2DPos                    (   ) const {return m_pos.truncateTo2D(); }

    /** 
    * @return const reference to the object's 3D position
    */
    ITF_INLINE  const Vec3d&    getPos                      (   ) const {return m_pos;}

    /** 
    * The anchor pos is the pos (global world coord) that is used by the editor to pick the object
    * @return the anchor pos
    */
    virtual     Vec3d           getAnchorPos                (   ) const { return getPos(); }

    /** 
    * Assigns a new X,Y position without changing the Z
    * @param _pos   the new X,Y position
    */
    virtual  void				set2DPos                    ( const Vec2d& _pos )       { ITF_ASSERT(_pos.isValid()); m_pos.m_x = _pos.m_x; m_pos.m_y = _pos.m_y; setModified();}

    /** 
    * Assigns a new X,Y,Z position
    * @param _pos   the new X,Y,Z position
    */
    virtual  void               setPos                      ( const Vec3d& _pos )       { ITF_ASSERT(_pos.isValid()); m_pos.m_x = _pos.m_x; m_pos.m_y = _pos.m_y; setDepth(_pos.m_z); setModified();}

    Vec3d                       getLocalPos                 (   ) const;
    void                        setLocalPos                 ( Vec3d _pos );

    Vec2d                       getLocal2DPos               (   ) const                 { return getLocalPos().truncateTo2D(); }
    void                        setLocal2DPos               ( Vec2d _pos );

    f32                         getLocalAngle               (   ) const;
    void                        setLocalAngle               ( f32 _angle );


    void                        setLocalInitialPos          ( const Vec3d& _pos );
    void                        setWorldInitialPos          ( const Vec3d& _pos );
    ITF_INLINE Vec3d            getLocalInitialPos          (   ) const { f32 myZ = getLocalInitialZ(); return m_initialPos.to3d(myZ); }
    Vec3d           			getWorldInitialPos          (   ) const;

    void                        setLocalInitialZ            ( f32 _z ) { m_initialZ = _z; }
    void                        setWorldInitialZ            ( f32 _z );
    f32                         getWorldInitialZ() const;
    ITF_INLINE virtual f32      getLocalInitialZ() const    { return m_initialZ; }

    void                        setLocalInitialRot          ( f32 _rot );
    void                        setWorldInitialRot          ( f32 _rot );

    ITF_INLINE f32              getLocalInitialRot          (   ) const { return m_initialRot; }
    f32                         getWorldInitialRot          (   ) const;

    void                        setLocalInitialScale        ( const Vec2d& _scale );
    void                        setWorldInitialScale        ( const Vec2d& _scale );

    ITF_INLINE virtual const Vec2d& getLocalInitialScale    (   ) const { return m_initialScale; }
    Vec2d                           getWorldInitialScale    (   ) const;

    virtual bbool               isZForced                   (   ) const { return bfalse; }


    u32                         m_dontUnloadResourceAtFrame;
    /**
    * Applies a translation to an object (adds a vector to its position)
    * @param    _delta                  the translation to add
    * @param    _applyToInitialValue    tells wether the new object's position should be retained as initial position (used to spawn the object)
    * @param    _updateForcedAttributes tells wether the forced attributes (layer relative z, forced scale, etc.) should be updated after applying the translation
    * <a href="#forcedattributes"> see forced attributes explanation</a>
    */
    virtual     void            offsetPosition              (   const Vec3d& _delta, bbool _applyToInitialValue );

    /**
    * Sets the absolute depth for an object. 
    * @param    _depth                  the new z (depth) for the object
    * <a href="#depthexplained"> see depth explanation</a>
    */
    virtual     void            setDepth                    (   f32 _depth  ) { m_pos.m_z = _depth; setModified(); }
    ITF_INLINE  f32             getDepth                    (   ) const { return m_pos.m_z; }

    /**
    * compute placement matrix from rotation and position
    * @param    _result             The resulting matrix (local to world)
    * @param    _ignoreScale        btrue if doesn't take scale into account. NB  : currently, bfalse is not supported.
    */
    void                        computePlacementMatrix      (   Matrix44 &_result, bbool _ignoreScale   ) const;

    ITF_INLINE u32              getObjectGroupIndex() const {return m_objectGroupIndex;}
    virtual u32                 addObjectInGroupForUpdate(ObjectGroup* _listToFill, bbool _fromParent = bfalse);
    virtual void                addObjectInGroupForDraw();

    ///////////////////////////////////////////////////////////////////////////
    /** End of Position section */
    ///////////////////////////////////////////////////////////////////////////





    ///////////////////////////////////////////////////////////////////////////
    /** Bounding Box section */
    ///////////////////////////////////////////////////////////////////////////
public:

    ITF_INLINE const AABB&		getRelativeAABB             (   ) const {return m_relativeAABB;}
    void            changeRelativeAABB             (   const AABB& _relativeAabb )
    { 
        ITF_ASSERT(_relativeAabb.isValid()); 
        if (m_isComputingNextAABB) 
            m_nextRelativeAABB = _relativeAabb; 
        else 
        {
            m_relativeAABB = _relativeAabb; 
            setModified();
        }
    }
    /**
    * Compute absolute AABB and return it.<br>
    * <b>Warning : this is not just an accessor, it actually computes
    * the world coordinates AABB from te local coordinates AABB whenever this method is called.</b><br>
    * @return the world coord AABB
    */
    ITF_INLINE  const AABB&     getAABB ( ) const
    {
        updateAbsoluteAABBFromRelative();
        return m_absoluteAABB;
    }

    /**
    * Refreshes the absolute (world coord) AABB from the relative one
    * This method is const despite the fact that it updates a member. It's because m_relativeAABB is mutalbe.
    * This is evil, and should be changed.    
    */
    ITF_INLINE void             updateAbsoluteAABBFromRelative() const
    {
        const AABB &aabb = getRelativeAABB();
        m_absoluteAABB.setMin( aabb.getMin() + get2DPos() );
        m_absoluteAABB.setMax( aabb.getMax() + get2DPos() );
    }

    /**
    * Set Relative AABB From Absolute AABB. NB : position must be valid
    * @param _absolute the new AABB. The relative aabb will be computed using current position
    */
    void                        changeRelativeAABBFromAbsolute(const AABB &_absolute) {changeRelativeAABB(AABB(_absolute.getMin()-get2DPos(), _absolute.getMax()-get2DPos()));}

    ///////////////////////////////////////////////////////////////////////////
    /** End of Bounding Box section */
    ///////////////////////////////////////////////////////////////////////////





    ///////////////////////////////////////////////////////////////////////////
    /** Editor section */
    ///////////////////////////////////////////////////////////////////////////
public:

    virtual     void        notifyParentsAndChildren (PickableEditorChangeType _type ) {};
    virtual     void        onChildChanged (ObjectRef _changedObject, PickableEditorChangeType _type) {};
    virtual     void        onParentChanged (ObjectRef _changedObject, PickableEditorChangeType _type) {};

#ifdef ITF_SUPPORT_EDITOR
    /**
    * This method is called whenever the object is moved by the editor (not by the gameplay).
    * --> when dragging / teleporting / rotating / scaling the object
    * --> when applying undo / redo on the object    
    * This method is expected to update the object's data (graphics, collision, etc.) according to the new position.
    * You should not rely on the next call to the object's update() method to update its data, because update is not called when the techno is paused and objects must remain editable (draggable, etc.) when paused.
    * It's important that most of the objects and components overload this method
    */
    virtual     void            onEditorMove                ( bbool _modifyInitialPos = btrue  ) {}

    /**
    * This method is called whenever an object is created by the editor (instanciation, drag'n'drop from a file, object cloning, sub-scene import, etc.)
    * It can be useful for some objects in case of duplication, because it passes the original object as a parameter and references to other objects can be handled gracefully.
    * @param _original  the object that was cloned (in case of object duplication)
    */
    virtual     void            onEditorCreated             ( Pickable* /*_original*/ = NULL ){}

    /*
    * Checks a list of conditions to tell wether the object should be displayed by the editor or not
    */
    bbool isHiddenBecauseOfEditor() const;

    bbool getHiddenForEditorFlag() const {return m_isHiddenForEditor;}
    void setHiddenForEditorFlag(bbool _hidden) {m_isHiddenForEditor = _hidden;}

    /** 
    * This information is used by the editor to show / hide different objects families (gameplay, background art, etc.)
    * The family information is read from the .act file for actors. For friezes, they are considered as gameplay elements if they have collisions
    * @return the object's family
    */
    ITF_INLINE  const u32       getObjectFamily             (   ) const {return  m_ObjectFamily;}


    ITF_INLINE PickingInfo2D*   getPickingInfo()const                   { return m_pickingInfo2D; }
    ITF_INLINE void             setPickingInfo(PickingInfo2D* _info2d)  { m_pickingInfo2D = _info2d; }

    void                    setResourceError( const String& _str )  { m_resourceErrorLabel = _str; setDataError(m_resourceErrorLabel); }
    const String&           getResourceErrorLabel() const           { return m_resourceErrorLabel; }
    void                    resetResourceError()                    { m_resourceErrorLabel.clear(); }
    bbool                   hasResourceError()const                 { return !m_resourceErrorLabel.isEmpty(); }

    const String&           getDataErrorLabel() const               { return m_dataErrorLabel; }

    void                    setObjectFamily(const u32 _family) {m_ObjectFamily = _family;}

    #endif //ITF_SUPPORT_EDITOR
    /**
    * Called when the object is picked
    * @param _info information about the picking context
    */
    virtual void            setSelected     (   PickingInfo2D* _info2D  );

    /**
    * Called for every object when picking is invoked.
    * @param _info information to fill.
    */
    virtual void            fillPickingInfo  (   PickingInfo2D* /*_info*/ ) { }

    ITF_INLINE void             setUserFriendly(const String8& _name)   { m_userFriendly = _name; }
    ITF_INLINE const String8&   getUserFriendly() const                 { return m_userFriendly; }
    ITF_INLINE bbool            isSelected() const                      { return m_isSelected; }

    /**
    * Reload the pickable from an archive
    * @param _archive (Archive *) the archive
    * @param _keepInstance (bbool) used to preserve instance values
    */
    virtual void                hotReloadFromMem( ArchiveMemory* _archive );

    /**
    * Finalizes the reload operations
    * @param _wasResLoaded (bbool)
    * @param _wasActive (bbool)
    * @param _wasActiveInScene (bbool)
    * @param _wasAllowedForCell (bbool)
    */
    virtual void                finalizeReload( bbool _wasResLoaded, bbool _wasActive, bbool _wasActiveInScene, bbool _wasAllowedForCell );








    /**
    * Generic entry point for destroying objects. 
    * Actual memory destruction will be processed later (objects are deleted by their scene).
    */
    virtual void                requestDestruction() {ITF_ASSERT_MSG(bfalse, "requestDestruction() should not be called for this kind of pickable");}

    /**
    * Clone the pickable and register it in the provided scene
    * @param _pScene the destination scene
    * @param _callOnLoaded a flag driving onLoaded() call
    * @return the new pickable. May return NULL.
    */
    virtual Pickable*           clone(Scene* /*_pScene*/, bbool /*_callOnLoaded*/)const { return NULL; }



    /**
    * rotates the object (z axis) around a given point
    * @param    _deltaAngle             the delta angle to add (radians)
    * @param    _applyToInitialValue    tells wether the new object's angle should be retained as initial angle (used to spawn the object)
    * @param    _pOrigin                pointer to the center of rotation. Uses the object's position if null
    */
    virtual     void            rotate                      (   f32 _deltaAngle, bbool _applyToInitialValue, const Vec3d* _pOrigin = NULL );
    virtual  void               setAngle(f32 _angle) { m_angle = _angle; setModified(); }
    ITF_INLINE  f32             getAngle() const  { return m_angle;}

    virtual void                scale( const Vec2d & _deltaScale, bbool _applyToInitialValue );
    ITF_INLINE const Vec2d&     getScale () const {return m_scale;}
    virtual void                setScale(const Vec2d & _scale );


    /**
    * An object that is NOT enabled has the following properties:
    * --> it won't be updated by its parent object if its binded (child)
    * --> it will remain within the scene's cells for resource operations: loadResources() and unloadResources() will be called depending on the camera
    * --> it will remain outside of the scene's cells for activation operations: onBecomeActive() and onBecomeInactive() will NOT be called
    * --> it won't be updated nor drawn by the scene : update() and draw() will NOT be called
    * @return wether the object is active inside its scene.
    */
    ITF_INLINE bbool            isEnabled() const {return m_isEnabled;}
    virtual void                enable();
    virtual void                disable();

    /**
    * @return btrue if onBecomeActive() was called on this object and onBecomeInactive() is not yet called
    * in other terms, tells if the object is within the cells activation range
    */
    ITF_INLINE bbool            isActive() const { return m_active; }


    /**
    * @param _alwaysVisible switch
    * setting _alwaysVisible to true makes visibility test always return true, event if bounding box is outside of the screen.
    * Note : the object can be considered as not visible in editor mode in some cases (see Pickable::isVisible implementation)
    */
    void setAlwaysVisible(bbool _alwaysVisible) {m_isAlwaysVisible=_alwaysVisible;}
    bbool isAlwaysVisible() const {return m_isAlwaysVisible;}




    /*
    * Determines wether an object is visible or not.
    * @return btrue if the object is visible, false otherwise
    */
    bbool isVisible(Camera* _camera) const;


    /**
    * This callback is invoked when the object has been instanciated and serialized.
    * At this moment, the objet just had its constructor and its serialization called, no other init is yet performed, and resources are not ready.
    * It's called:
    * - when the scene that owns the object has finished loading
    * - when the object is spawned by a spawner
    * - when the object is cloned (when it's the clone of another object)
    * - when the object is hot reloaded
    * It mainly:
    * - calls onActorLoaded() for all components (for actors)
    * - resets the position / rotation / scale to the initial values (for all pickables)
    * - WARNING : You MUST make sure that it computes a first valid AABB for the object so that it's properly registered in cells
    */
    enum HotReloadType
    {
        HotReloadType_None = 0,
        HotReloadType_Checkpoint,
        HotReloadType_Default
    };
   
    virtual void            onLoaded(HotReloadType _hotReload = HotReloadType_None);   
    virtual bbool           onLoadedFromScene(HotReloadType _hotReload = HotReloadType_None);   

    /**
    * Asks the object to querry for its resources. Basically, it adds a physical user to the object's resources group.
    * Resources are not guaranteed to be ready (loaded in RAM) when exiting this method (this is just a loading querry)
    * This method is mainly invoked by the cells system when the object enters the loading area (distance around the camera)
    * It can also be invoked in some specific spawning cases:
    * - ghost activation
    * - spawned by spawner
    * - always active actor / 2d actor registration
    */
    virtual void    loadResources();
    virtual void    unloadResources();

    ITF_INLINE bbool        isPhysicalReady() const         { return m_physicalReady; }
    ITF_INLINE  void        setPhysicalReady(bbool _physicalReady)    {m_physicalReady = _physicalReady;}
    
    virtual bbool           updatePhysicalReady()           { return isPhysicalReady(); }

    /**
    * Returns the reference depth of the pickable, the final z of the pickable is reference + (forcedZ or relativeZ)
    * If the pickable is inside a subscene then the subscene depth is the reference.
    * Reference is 0.0f for the main scene
    * @return the depth reference
    */
    f32                     getReferenceDepth()const;

    ResourceGroup*          getResourceGroup_Slow();

    ITF_INLINE u32          getLastActiveFrame() const {return m_lastActiveFrame;}
    ITF_INLINE void         setLastActiveFrame(u32 _frame) {m_lastActiveFrame = _frame;}
    ITF_INLINE void         setLastVisibleFrame(u32 _frame) {m_lastVisibleFrame = _frame;}
    ITF_INLINE u32          getLastVisibleFrame() {return m_lastVisibleFrame;}

    /**
    * Sets the object as active for a given frame --> means it's in the cells activation range
    */
    void                    setActiveForFrame(u32 _frame);

    virtual                 void update(f32 /*_deltaTime*/) {ITF_ASSERT(0);}
    virtual                 void draw() {ITF_ASSERT(0);}
    virtual                 void draw2D() {}

    virtual void            onEvent(class Event* /*_event*/) {}

    virtual void            onSceneActive()     {};
    virtual void            onSceneInactive()   {};

    /**
    *   Implementation of method setActiveForFrame() guarantees that the objet has its resources loaded when onbecomeActive() is called
    */
    virtual void            onBecomeActive();
    virtual void            onBecomeInactive();

    void                    setAllowedCell(bbool _isAllowedForCell);
    bbool                   isAllowedForCell() const                        {return m_isAllowedForCell;}

    ITF_INLINE bbool        isRequestedLoading() const { return m_bRequestedLoading;}
    
    void                    resetCurrentInfos();
    virtual  void           storeCurrentToInitial() { setWorldInitialPos(getPos()); setWorldInitialRot(getAngle()); setWorldInitialScale(getScale()); }

	bbool					isOnScreen();

    Scene*                  getScene() const;
    World*                  getWorld() const;

    virtual void            onAddedToScene(Scene* _pScene);
    virtual void            onRemovedFromScene(Scene* _pScene,bbool _forDeletion);

    void                    updateCellUpdateData(const AABB& _aabb, u8 _layerIndex);
    bbool                   updateCellsNeeded(const AABB& _aabb, u32 _layerIndex );
    void                    resetCellUpdateInfo();

    /**
    * Set the container scene apply some scene's properties (disconnected from cells...)
    * @param _scene The new container scene
    */
    void                    setScene( class Scene* _scene );
    ITF_INLINE ObjectRef    getSceneRef() const {return  m_sceneRef;}
    

    /**
    * deletionRequest is called by unregistration functions in scene
    */
    void                    deletionRequest()               { m_deletionRequested = btrue; }
    ITF_INLINE bbool        isDeletionRequested() const     { return m_deletionRequested; }

    void                    setSerializable(bbool _b)       { m_isSerializable = _b; }
    bbool                   isSerializable()const           { return m_isSerializable; }
    
    /**
    * generate and set a new user friendly of type : "prefix@123456" (the number is pseudo random)
    * @param _prefix The prefix of the new friendly, if empty the current friendly will be used
    */
    void                    generateNewUserFriendly(const String8& _prefix = "");

    /**
    * register all modified pickable
    */
    static void             registerAllModifiedInWorldCells();

    virtual void            setDataError (const String& _str);
    void                    resetDataError();
    ITF_INLINE bbool        hasDataError() const                    { return m_bHasDataError; }
    const CellUpdateData&   getLastCellUpdateData() const {ITF_ASSERT(isLastCellUpdateDataValid()); return m_lastCellUpdateData;}
    bbool                   isLastCellUpdateDataValid() const {return m_lastCellUpdateData.m_aabb.isValid();}
    virtual const Path*     getTemplatePath() const {return NULL;}

    void                    setObjectGroupIndex( u32 _index)    {m_objectGroupIndex = _index;}

    /*
    * Registers the object in all the cells that cross with its bounding box (cross or contain or are contained within its bounding box)
    * Called manually during loading/init process but automatically in runtime.
    */
    void                    registerInWorldCells();
    /*
    * Unregisters object from cells so that cells don't reference a destroyed object anymore. Called when the object is destroyed or reloaded
    */
    void                    unregisterFromWorldCells();

protected:
    String8                 m_userFriendly;
    AABB                    m_relativeAABB;
    AABB                    m_nextRelativeAABB;
    mutable AABB            m_absoluteAABB; //m_absoluteAABB is only a temporary variable used by getAABB()
    bbool                   m_bRequestedLoading;
    Vec3d                   m_pos;
    Vec2d                   m_initialPos;
    f32                     m_initialZ;
    f32                     m_initialRot;
    f32                     m_angle;
    Vec2d                   m_initialScale;
    Vec2d                   m_scale;

    bbool                   m_isSerializable;
    bbool                   m_deletionRequested;
    bbool                   m_isComputingNextAABB;

    /*
    * <a href="#forcedattributes"> see forced attributes explanation</a>
    */

    ITF_INLINE void         setActive(bbool _active)    {m_active  = _active;};


#ifdef ITF_SUPPORT_EDITOR
    PickingInfo2D*                              m_pickingInfo2D;    // used only by the editor
#endif //ITF_SUPPORT_EDITOR

private:

    void                    preChangeProperties();
    void                    postChangeProperties();

    void                    initializePosAngleScaleFromInitialValues();

    ObjectRef               m_sceneRef;

    static SafeArray<PickablePropertyEditState> m_PropertyEditStates;
    static i32                                  getPropertyEditStateIndex(const ObjectRef _ref);

    bbool                                       m_modified;
    void                                        setModified();
    static SafeArray<ObjectRef>                 m_modifiedList;
    u32                                         m_objectGroupIndex;

#ifdef ITF_SUPPORT_EDITOR
    String                                      m_dataErrorLabel;
    String                                      m_resourceErrorLabel;
    u32                                         m_ObjectFamily; ///< test objectfamily
    bbool                                       m_isHiddenForEditor;
#endif //ITF_SUPPORT_EDITOR
};

} // namespace ITF




#endif // _ITF_PICKABLE_H_
