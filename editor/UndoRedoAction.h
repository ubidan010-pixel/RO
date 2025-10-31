#ifndef _ITF_UNDOREDOACTION_H_
#define _ITF_UNDOREDOACTION_H_

#ifndef _ITF_ARCHIVEMEMORY_H_
#include "core/ArchiveMemory.h"
#endif //_ITF_ARCHIVEMEMORY_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

namespace ITF
{
    class UndoRedoAction
    {
    public:
        enum ActionType     { preChange, preDelete, postAdd, preTemplate, sceneMigration };
        enum SequenceStep   { First, Second, Count };

        UndoRedoAction(ActionType _type) : m_actionType(_type) {}
        virtual ~UndoRedoAction() {}

        virtual void    applyAsUndo(SequenceStep _step) = 0;
        virtual void    applyAsRedo(SequenceStep _step) = 0;

        virtual u32     getMemoryUsage()const = 0;

#if !defined(ITF_FINAL)
        const String&   getDebugInfo()const         { return m_debugInfo; }
#endif // !ITF_FINAL

    protected:
        ActionType      m_actionType;

#if !defined(ITF_FINAL)
        String          m_debugInfo;
#endif // !ITF_FINAL
    };

    //////////////////////////////////////////////////////////

    class UndoRedoActionSingle : public UndoRedoAction
    {
    public:

        UndoRedoActionSingle(ActionType _type, Pickable* _pPickable);
        virtual ~UndoRedoActionSingle() {}

        virtual void    applyAsUndo(SequenceStep _step) = 0;
        virtual void    applyAsRedo(SequenceStep _step) = 0;

        virtual u32     getMemoryUsage()const = 0;

        Scene*          getContextScene()const;
        Pickable*       getPickable()const;

    protected:
        ObjectPath      m_pickableAbsolutePath;
        bbool           m_pickableIsSelected;
        bbool           m_pickableWasHiddenBeforeUndo;
        bbool           m_pickableWasHiddenBeforeRedo;
        bbool           m_isSubSceneActor;
    };

    //////////////////////////////////////////////////////////

    class UndoRedoAction_preChange : public UndoRedoActionSingle
    {
    public:
        UndoRedoAction_preChange( Pickable* _pPickable);
        virtual ~UndoRedoAction_preChange();

        virtual void    applyAsUndo(SequenceStep _step);
        virtual void    applyAsRedo(SequenceStep _step);

        virtual u32     getMemoryUsage()const;

    private:
        ArchiveMemory* m_pickableContentBeforeUndo;
        ArchiveMemory* m_pickableContentBeforeRedo;
    };
    
    //////////////////////////////////////////////////////////

    class UndoRedoAction_preDelete : public UndoRedoActionSingle
    {
    public:
        UndoRedoAction_preDelete( Pickable* _pPickable, bbool _delayOnLoaded );

        virtual void    applyAsUndo(SequenceStep _step);
        virtual void    applyAsRedo(SequenceStep _step);

        virtual u32     getMemoryUsage()const;

        static void     addPickable(Scene* _pScene, Pickable* _pPickable);

    private:
        ArchiveMemory   m_pickableContent;
        ObjectRef       m_pickableAdded;
        bbool           m_delayOnLoaded;
    };
    
    //////////////////////////////////////////////////////////

    class UndoRedoAction_postAdd : public UndoRedoActionSingle
    {
    public:
        UndoRedoAction_postAdd( Pickable* _pPickable, bbool _delayOnLoaded );

        virtual void    applyAsUndo(SequenceStep _step);
        virtual void    applyAsRedo(SequenceStep _step);

        virtual u32     getMemoryUsage()const;

        static void     deletePickable(Scene* _pScene, Pickable* _pPickable);

    private:
        ArchiveMemory   m_pickableContent;
        bbool           m_contentSaved;
        ObjectRef       m_pickableAdded;
        bbool           m_delayOnLoaded;
    };
    
    //////////////////////////////////////////////////////////

    class UndoRedoAction_sceneMigration : public UndoRedoActionSingle
    {
    public:
        UndoRedoAction_sceneMigration(Pickable* _pPickable, const ObjectPath& _previousSceneAbsolutePath);

        virtual void    applyAsUndo(SequenceStep _step);
        virtual void    applyAsRedo(SequenceStep _step);

        virtual u32     getMemoryUsage()const;

    private:
        void            revertMigration();
        ObjectPath      m_SrcScenePath, m_DstScenePath;
    };
   
    //////////////////////////////////////////////////////////
    // Undo Redo action for templates !!!
    // ObjectRef is here to Be sure that the template pointer is ok

    class TemplateObj;

    class UndoRedoActionTemplate : public UndoRedoAction
    {
    public:

        UndoRedoActionTemplate(TemplateObj* _template, ObjectRef _actorRef, processObjectRef _callback);
        virtual ~UndoRedoActionTemplate();

        virtual void    applyAsUndo(SequenceStep _step);
        virtual void    applyAsRedo(SequenceStep _step);

        virtual u32     getMemoryUsage()const;

    protected:
        ObjectRef               m_actorRef;
        TemplateObj*            m_pTemplate;
        processObjectRef   m_callback;

        ArchiveMemory* m_templateContentBeforeUndo;
        ArchiveMemory* m_templateContentBeforeRedo;
    };

} // namespace ITF

#endif // _ITF_UNDOREDOACTION_H_