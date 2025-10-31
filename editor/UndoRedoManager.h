#ifndef _ITF_UNDOREDOMANAGER_H_
#define _ITF_UNDOREDOMANAGER_H_

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

#ifndef _ITF_WORLD_EVENT_INTERFACE_H_
#include "engine/scene/worldEventInterface.h"
#endif //_ITF_WORLD_EVENT_INTERFACE_H_

namespace ITF
{

    class Scene;
    class ObjectPath;
    class SubSceneActor;
    class UndoRedoAction;
    class UndoRedoSequence;
    struct UndoRedoWorldData;

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// UndoRedoManager class
    class UndoRedoManager : public Interface_WorldEventsListener,public TemplateSingleton<UndoRedoManager>
    {
    public:

        UndoRedoManager();

        virtual ~UndoRedoManager();

        void clear(bbool _deleteSequences);
        
        ITF_INLINE bbool isEnabled() const  { return m_bManagerEnabled; }
        void setEnabled(bbool _bstate)      { m_bManagerEnabled = _bstate; }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Back to last undo point (request applied at next update)
        ITF_INLINE void requestUndo()               { m_bUndoRequested = btrue ; }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Request a redo action (request applied at next update)
        ITF_INLINE void requestRedo()               { m_bRedoRequested = btrue ; }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Update function used to apply undo if requested
        void update();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Start a sequence of undo records, so a validated sequence will be considered as one undo point
        /// For exemple, if the user delete a multiple selection we want one CTRL-Z to re-add the whole selection
        /// @param _sequenceDescription A description of the sequence of action (sample : "Delete the selection")
        void beginUndoRedoSequence(const String8& _sequenceDescription);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Validate the current undo sequence
        void validateUndoRedoSequence(const String8& _sequenceDescription);
        
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Discard the current undo sequence
        void cancelUndoRedoSequence(const String8& _sequenceDescription);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Save the state of a pickable just before it is changed
        /// @param _pPickable The pickable to be saved
        void preChange(Pickable* _pPickable);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Save the state of a pickable just before it is deleted
        /// @param _pPickable The pickable to be saved
        void preDelete(Pickable* _pPickable);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Save the add event for a particular pickable
        /// @param _pPickable The pickable just added
        void postAdd(Pickable* _pPickable);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Save the state of a group just before it is deleted
        /// @param _group The group to be saved
        void preDeleteGroup(SubSceneActor* _group);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Save the add event for a particular group
        /// @param _group The group just added
        void postAddGroup(SubSceneActor* _group);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Ssave the post scene migration action
        /// @param _pPickable The pickable just migrated
        /// @param _previousAbsolutePath The previous absolute path before the migration
        void postSceneMigration(Pickable* _pPickable, const ObjectPath& _previousAbsolutePath);
        
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Save the state of a template just before it is changed
        /// @param _template The template to be saved
        /// @param _actorRef The selected actor using this template
        void preChangeTemplate(class TemplateObj* _template, ObjectRef _actorRef, processObjectRef _callback);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// If the context changed, clear all undo datas
        /// @param _pWorld The switched world
        /// @param _bSwitchDone Tell if it's before or after the switch (so _pWorld it's the old world or the new one)
        virtual void onSwitchToWorld(World* _pWorld, bbool _bSwitchDone);
        virtual void onDeleteWorld(World* /*_pWorld*/) {};
        virtual void onStartDrawProcess(){}; // inherited from Interface_WorldEventsListener
        virtual void onEndDrawProcess(){}; // inherited from Interface_WorldEventsListener
        virtual void onStartUpdateProcess(f32 /*_dt*/) {}; // inherited from Interface_WorldEventsListener
        virtual void onEndUpdateProcess(f32 /*_dt*/) {}; // inherited from Interface_WorldEventsListener
        virtual void onUpdateProcess(f32 /*_dt*/){}; // inherited from Interface_WorldEventsListener

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Get memory usage
        /// @return the memory usage in byte
        u32 getMemoryUsage()const;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Dump the manager to a file
        /// @param _absPath the absolute path to the output file
        /// @return if the operation succeed
        bbool dumpToFile(const String8& _absPath)const;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Tell to this manager that the current world was just saved
        void currentWorldJustSaved()                                        { m_lastSaveMarker = m_undoRedoMarker; }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Check that the last save was done before the last change
        /// @return btrue if there is some unsaved changes
        bbool thereIsSomeModificationSinceLastSave()const                   { return m_lastSaveMarker != m_undoRedoMarker; }

        // Accessors, mainly used to get current world info for tabs system
        void fillWorldData(UndoRedoWorldData& _worldData)const;
        void applyWorldData(UndoRedoWorldData& _worldData);

    private:

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Internal function to simplify the registration of a new undoAction
        /// @param _action the new undo action to register
        void                                registerNewUndoRedoAction(UndoRedoAction* _action);

        void                                applyUndo();
        void                                applyRedo();

        bbool                               m_bManagerEnabled;
        bbool                               m_bUndoRequested;
        bbool                               m_bRedoRequested;

        ///////////////////////////////////////////////////////////////////////////////////////////

        UndoRedoSequence*                   m_currentUndoRedoSequence;
        ITF_VECTOR<UndoRedoSequence*>       m_undoRedoSequenceList;
        u32                                 m_undoRedoMarker;

        u32                                 m_lastSaveMarker;
    };
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// UndoRedoSequence class
    class UndoRedoSequence
    {
    public:
        UndoRedoSequence(const String8& _name);
        ~UndoRedoSequence();

        void            applyAsUndo();
        void            applyAsRedo();

        void            setName(const String8& _name)       { m_name = _name; }
        const String8&  getName()const                      { return m_name; }

        u32             getActionCount()const               { return m_actionList.size(); }
        UndoRedoAction* getActionAt(u32 _index)const        { return m_actionList[_index]; }
        void            addAction(UndoRedoAction* _pAct)    { m_actionList.push_back(_pAct); }

        u32             getMemoryUsage()const;

    private:
        String8                     m_name;
        SafeArray<UndoRedoAction*>  m_actionList;

        void                        updateDebugInfo(); // Only effective in !FINAL
        void                        destroy();
    };

    ///////////////////////////////////////////////////////////////////////////////////////////

    struct UndoRedoWorldData
    {
        UndoRedoWorldData()
            : m_marker(0)
            , m_lastSaveMarker(0)
        {
        }

        ITF_VECTOR<UndoRedoSequence*>  m_sequenceList;
        u32                                             m_marker;
        u32                                             m_lastSaveMarker;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Helper class to securise code
    class UndoRedoSequenceScope
    {
    public:
        UndoRedoSequenceScope(String8 _name);
        ~UndoRedoSequenceScope();

        void cancel();

    private:
        String8 m_Name;
        bbool   m_Cancelled;
    };
   
#define UNDOREDO_MANAGER UndoRedoManager::getptr()
} // namespace ITF

#endif // _ITF_UNDOREDOMANAGER_H_