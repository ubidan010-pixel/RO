#include "precompiled_editor.h"

#ifndef _ITF_EDITORCONTEXTMENU_H_
#include "editor/EditorContextMenu.h"
#endif //_ITF_EDITORCONTEXTMENU_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

namespace ITF
{

SafeArray<EditorContextMenu::ContextMenuInfo> EditorContextMenu::m_menus;

void EditorContextMenu::setMenuExclusive(EditorContextMenu* _menu, bbool _exclusive)
{
    ContextMenuInfo* info = getMenuInfo(_menu);
    if (!info)
        return;

    info->m_isExclusive = _exclusive;
}

void EditorContextMenu::registerMenu(EditorContextMenu* _menu, bbool _isExclusive)
{
    ContextMenuInfo& info = m_menus.incrSize();
    info.m_menu = _menu;
    info.m_isExclusive = _isExclusive;
}

void EditorContextMenu::keepOnlyOneMenuOpen(EditorContextMenu* _menu)
{
    for (u32 i = 0; i < m_menus.size(); i++)
    {
        if (m_menus[i].m_menu != _menu)
        {
            m_menus[i].m_menu->fold();
            m_menus[i].m_allowDraw = bfalse;
        }
    }
}

void EditorContextMenu::onOpenMenu(EditorContextMenu* _menu)
{
    ContextMenuInfo* info = getMenuInfo(_menu);
    if (!info)
        return;

    if (info->m_isExclusive)
    {
        keepOnlyOneMenuOpen(_menu);
        return;
    }
}

EditorContextMenu::ContextMenuInfo* EditorContextMenu::getMenuInfo(EditorContextMenu* _menu)
{
    for (u32 i = 0; i < m_menus.size(); i++)
    {
        if (m_menus[i].m_menu == _menu)
        {
            return &m_menus[i];
            break;
        }
    }
    return NULL;
}

void EditorContextMenu::deleteMenuInfo(EditorContextMenu* _menu)
{
    for (u32 i = 0; i < m_menus.size(); i++)
    {
        if (m_menus[i].m_menu == _menu)
        {
            m_menus.eraseNoOrder(i);
            break;
        }
    }
}

EditorContextMenu::EditorContextMenu():m_pos(Vec2d::Zero),m_launchPos(Vec2d::Zero)
{
    m_maxChar = 0;
    m_lastTarget = ITF_INVALID_OBJREF;
}

EditorContextMenu::~EditorContextMenu()
{
    clearItems();
    deleteMenuInfo(this);
}

ContextMenuItem* EditorContextMenu::getItem(ItemId _Id)
{
    ITF_VECTOR<ContextMenuItem*>::iterator it = m_items.begin();
    ITF_VECTOR<ContextMenuItem*>::iterator end = m_items.end();

    for (; it != end; ++it)
    {
        ContextMenuItem& _item = *(*it);
        if (_item.isExternal())
        {
            if ((u32)_item.getIdFromExternal() == (u32)_Id)
            {
                return &_item;
            }
        }
        else
        {
            if (_item.getId() == _Id)
            {
                return &_item;
            }
        }
    }
    return NULL;
}


ContextMenuItem* EditorContextMenu::getItem(const String& _label)
{
    ITF_VECTOR<ContextMenuItem*>::iterator it = m_items.begin();
    ITF_VECTOR<ContextMenuItem*>::iterator end = m_items.end();

    for (; it != end; ++it)
    {
        ContextMenuItem& _item = *(*it);
        if (_item.m_label == _label)
        {
            return &_item;
        }
    }
    return NULL;
}

void EditorContextMenu::fold()
{
    ITF_VECTOR<ContextMenuItem*>::iterator it = m_items.begin();
    ITF_VECTOR<ContextMenuItem*>::iterator end = m_items.end();

    for (; it != end; ++it)
    {
        ContextMenuItem& _item = *(*it);
        _item.m_bShowSubMenu = bfalse;
    }
}
 
ContextMenuItem* EditorContextMenu::addSubItem(ItemId _Id, const String& _what, ItemId _whereId, const String& _where, bbool _isSeparator)
{    
    ContextMenuItem* insertItem = addItem(_whereId, _where);
    if (insertItem)
    {
        return insertItem->m_subMenu.addItem(_Id, _what, _isSeparator);
    }
    return NULL;
}

f32 EditorContextMenu::getLineHeight()
{
    return Editor::fCharMaxHeight_Big * 1.5f;
}

f32 EditorContextMenu::getLineInterstice()
{
    return 2.f;
}

f32 EditorContextMenu::getSubMenuInterstice()
{
    return 5.f;
}

ContextMenuItem* EditorContextMenu::addItemFromExternalPlugin(u32 _Id, const String& _label, bbool _isSeparator)
{
    ContextMenuItem* it = addItem(EditorContextMenu::ItemId(_Id), _label, _isSeparator);
    it->setExternal();
    return it;
}

ContextMenuItem* EditorContextMenu::addSubItemFromExternalPlugin(u32 _Id, const String& _what, u32 _whereId, const String& _where, bbool _isSeparator)
{
    ContextMenuItem* it = addSubItem(EditorContextMenu::ItemId(_Id), _what, EditorContextMenu::ItemId(_whereId), _where, _isSeparator);
    it->setExternal();
    return it;
}

ContextMenuItem* EditorContextMenu::addItem(ItemId _Id, const String& _label, bbool _isSeparator)
{
    ContextMenuItem* item = getItem(_Id);

    if (item && item->m_label == _label && _Id != ItemId_None)
        return item;

    m_lastAABB.setMin(m_pos);
    m_lastAABB.setMax(m_pos);

    if (m_maxChar < (i32)_label.getLen())
        m_maxChar = _label.getLen() + 2;

    {
        item = new ContextMenuItem();
        item->m_pos.m_x = 0;
        item->m_size = Vec2d(_label.getLen() * Editor::fCharMaxWidth_Big + Editor::fTextSubOffset_Big, getLineHeight());
        if (_isSeparator)
            item->m_size.m_y *= 0.5f;
        item->m_r = 1.f;
        item->m_g = 1.f;
        item->m_b = 1.f;
        item->m_backR = 0.5f;
        item->m_backG = 1.0f;
        item->m_backB = 0.5f;
        item->m_label = _label;
        item->m_bShowSubMenu = bfalse;
        item->setId(_Id);
        item->m_isSeparator = _isSeparator;
        m_items.push_back(item);
    }

    ITF_VECTOR<ContextMenuItem*>::iterator it = m_items.begin();
    ITF_VECTOR<ContextMenuItem*>::iterator end = m_items.end();
//    u32 count = m_items.size();
    f32 curY = 0.f;

    for (; it != end; ++it)
    {
        ContextMenuItem* _item = *it;
        _item->m_size.m_x = m_maxChar * Editor::fCharMaxWidth_Big + Editor::fTextSubOffset_Big;
        _item->m_pos.m_y = curY;
        curY += _item->m_size.m_y + getLineInterstice();
    }

    return item;
}

f32     EditorContextMenu::getWidth()
{
    if (m_items.size()) 
        return m_items[0]->m_size.m_x; 
    return 0.f;
}


void    EditorContextMenu::getAABB(AABB& _aabb)
{
    _aabb =  m_lastAABB;
    Vec2d min = _aabb.getMin();
    if (min.m_x < 0.f)
        min.m_x = 0.f;
    if (min.m_y < 0.f)
        min.m_y = 0.f;
    _aabb.setMin(min);
}

void    EditorContextMenu::fitScreen(EditorContextMenu* _parent )
{
    if (!m_items.size())
        return;

    const f32 Vmargin = 10.f;
    const f32 Hmargin = 0.f;

    ITF_VECTOR<ContextMenuItem*>::iterator it = m_items.begin();
    ITF_VECTOR<ContextMenuItem*>::iterator end = m_items.end();

    f32 w = (f32)GFX_ADAPTER->getScreenWidth();
    f32 h = (f32)GFX_ADAPTER->getScreenHeight();
    f32 delta = (m_pos.m_x + m_items[0]->m_size.m_x) - (w - Hmargin);
    if (delta > 0.f)
    {
        if (_parent)
            m_pos.m_x = _parent->m_pos.m_x - m_items[0]->m_size.m_x;
        else
            m_pos.m_x = w - Hmargin - m_items[0]->m_size.m_x;
    }
    
    if (m_pos.m_y < Vmargin + Editor::fCharMaxHeight_Big * 2.f)
        m_pos.m_y = Vmargin + Editor::fCharMaxHeight_Big * 2.f;

    delta = (m_pos.m_y + m_items[m_items.size()-1]->m_pos.m_y) - (h - Vmargin - Editor::fCharMaxHeight_Big * 2.f);
    if (delta > 0.f)
        m_pos.m_y -= delta;
}

void    EditorContextMenu::draw(Vec2d _mousePos, EditorContextMenu* _parent, Vec2d* _forcePos )
{
    if (!m_items.size())
        return;

    bbool isUnfolded = bfalse;

    i32 saveHide = EDITOR->m_hide2d;
    EDITOR->m_hide2d = 0;
    u32 saveGFXHide = GFX_ADAPTER->getHideFlags();
    GFX_ADAPTER->setHideFlags(0);

    Vec2d localPos = m_pos;
    if (_forcePos)
        localPos = *_forcePos;

    m_lastAABB.setMin(localPos);
    m_lastAABB.setMax(localPos);

//    fitScreen(_parent);

    ITF_VECTOR<ContextMenuItem*>::iterator it = m_items.begin();
    ITF_VECTOR<ContextMenuItem*>::iterator end = m_items.end();

    EditorContextMenu* underMouseMenu = NULL;
    ContextMenuItem* underMouse = getItemAtPos(_mousePos, &underMouseMenu);

    u32 uSeparatorColor     = ITFCOLOR_TO_U32(150, 50, 50, 50);
    u32 uUnfoldColor        = ITFCOLOR_TO_U32(220, 50, 50, 150);

    for (; it != end; ++it)
    {
        ContextMenuItem& _item = *(*it);

        u32 uColor     = ITFCOLOR_TO_U32(200, (i32)(_item.m_backR * 50.0f), (i32)(_item.m_backG * 50.0f), (i32)(_item.m_backB * 50.0f));
        u32 uDarkColor = ITFCOLOR_TO_U32(200, (i32)(_item.m_backR * 20.f), (i32)(_item.m_backG * 20.f), (i32)(_item.m_backB * 20.f));
        u32 uSelColor     = ITFCOLOR_TO_U32(200, (i32)(_item.m_backR * 100.0f), (i32)(_item.m_backG * 100.0f), (i32)(_item.m_backB * 100.0f));
        u32 uSelDarkColor = ITFCOLOR_TO_U32(200, (i32)(_item.m_backR * 50.f), (i32)(_item.m_backG * 50.f), (i32)(_item.m_backB * 50.f));
        u32 uUseColor     = uColor;
        u32 uUseDarkColor = uDarkColor;

        Vec2d pos = localPos + _item.m_pos;

        if (underMouseMenu && underMouseMenu == this)
            if (underMouse != &_item)
                _item.m_bShowSubMenu = bfalse;  // same menu, not same item --> hide submenu

        if (underMouseMenu && underMouseMenu != this && underMouseMenu != &_item.m_subMenu)
                _item.m_bShowSubMenu = bfalse;  // not same menu, not submenu --> hide submenu

        // Note: if underMouseMenu is NULL, submenus should not be hidden

        if (underMouse == &_item || _item.m_bShowSubMenu)
        {
            isUnfolded    = btrue;
            uUseColor     = uSelColor;
            uUseDarkColor = uSelDarkColor;
            if (_item.m_subMenu.m_items.size())
            {
                _item.m_subMenu.m_pos = pos;
                _item.m_subMenu.m_pos.m_x += _item.m_size.m_x + getSubMenuInterstice();
                GFX_ADAPTER->draw2dBox(_item.m_subMenu.m_pos - Vec2d(getSubMenuInterstice(), getLineInterstice()-_item.m_size.m_y*0.5f), getSubMenuInterstice(), getLineInterstice()*2.f, uColor, uColor, uSelColor, uSelColor);
                _item.m_subMenu.m_pos.m_y += _item.m_size.m_y * 0.15f;
                _item.m_bShowSubMenu = btrue;
                _item.m_subMenu.draw(_mousePos, this);
                m_lastAABB.grow(_item.m_subMenu.m_lastAABB);
            }
        }

        
        if (_item.m_isSeparator)
        {/*
            f32 localY = pos.m_y;
            while(localY < pos.m_y + _item.m_size.m_y)
            {
                GFX_ADAPTER->draw2dBox(Vec2d(pos.m_x, localY), _item.m_size.m_x, getLineInterstice(), uSeparatorColor, uSeparatorColor, uSeparatorColor, uSeparatorColor);
                localY += getLineInterstice() * 2.f;
            }*/
//            GFX_ADAPTER->draw2dBox(pos + Vec2d(0.f, getLineInterstice()*2.f), _item.m_size.m_x, _item.m_size.m_y - getLineInterstice()*4.f, uSeparatorColor, uSeparatorColor, uSeparatorColor, uSeparatorColor);
            GFX_ADAPTER->draw2dBox(pos, _item.m_size.m_x, _item.m_size.m_y, uSeparatorColor, uSeparatorColor, uSeparatorColor, uSeparatorColor);
        }
        else
        {
            GFX_ADAPTER->draw2dBox(pos, _item.m_size.m_x, _item.m_size.m_y, uUseColor, uUseColor, uUseDarkColor, uUseColor);
            if (_item.m_subMenu.m_items.size() && !_item.m_bShowSubMenu)
            {
                GFX_ADAPTER->draw2dBox(pos + Vec2d(_item.m_size.m_x*0.86f, _item.m_size.m_y*0.37f), _item.m_size.m_x*0.06f, _item.m_size.m_y*0.1f, uUnfoldColor, uUnfoldColor, uUnfoldColor, uUnfoldColor); // horiz
                GFX_ADAPTER->draw2dBox(pos + Vec2d(_item.m_size.m_x*0.88f, _item.m_size.m_y*0.24f), _item.m_size.m_x*0.015f, _item.m_size.m_y*0.35f, uUnfoldColor, uUnfoldColor, uUnfoldColor, uUnfoldColor); // vert
            }
        }
        GFX_ADAPTER->drawDBGText(_item.m_label, pos.m_x + Editor::fTextSubOffset_Big, pos.m_y, _item.m_r, _item.m_g, _item.m_b, btrue);

        m_lastAABB.grow(m_pos + _item.m_pos + _item.m_size);
    }

    GFX_ADAPTER->setHideFlags(saveGFXHide);
    EDITOR->m_hide2d = saveHide;

    if(isUnfolded && !_parent)
        onOpenMenu(this);
}

ContextMenuItem*  EditorContextMenu::getItemAtPos(const Vec2d& _pos, EditorContextMenu** _foundIn)
{
    if (_foundIn)
        *_foundIn = NULL;

    ITF_VECTOR<ContextMenuItem*>::iterator it = m_items.begin();
    ITF_VECTOR<ContextMenuItem*>::iterator end = m_items.end();

    for (; it != end; ++it)
    {
        ContextMenuItem& _item = *(*it);
        if (_item.m_isSeparator)
            continue;
        if (_item.m_bShowSubMenu)
        {
            ContextMenuItem* sub = _item.m_subMenu.getItemAtPos(_pos);
            if (sub)
            {
                if (_foundIn)
                    *_foundIn = &_item.m_subMenu;
                return sub;
            }
        }
        Vec2d pos = m_pos + _item.m_pos;
        if ((_pos.m_x > pos.m_x) && (_pos.m_x < pos.m_x + _item.m_size.m_x) && (_pos.m_y > pos.m_y) && (_pos.m_y < pos.m_y + _item.m_size.m_y))
        {
            if (_foundIn)
                *_foundIn = this;
            return &_item;
        }
    }
    return NULL;
}


ContextMenuItem* EditorContextMenu::onLeftMB(Vec2d _mousePos)
{
    return getItemAtPos(_mousePos);
}

bbool EditorContextMenu::isFolded() const
{
    ITF_VECTOR<ContextMenuItem*>::const_iterator it = m_items.begin();
    ITF_VECTOR<ContextMenuItem*>::const_iterator end = m_items.end();

    for (; it != end; ++it)
    {
        if ((*it)->m_bShowSubMenu)
            return bfalse;
    }
    return btrue;
}

void    EditorContextMenu::clearItems()
{
    ITF_VECTOR<ContextMenuItem*>::iterator it = m_items.begin();
    ITF_VECTOR<ContextMenuItem*>::iterator end = m_items.end();

    for (; it != end; ++it)
    {
        (*it)->m_subMenu.clearItems();
        delete (*it);
    }
    m_items.clear();
    m_maxChar = 0;
}

void    EditorContextMenu::setPos(const Vec2d& _pos)
{
    m_pos = _pos;
}

} //namespace ITF

