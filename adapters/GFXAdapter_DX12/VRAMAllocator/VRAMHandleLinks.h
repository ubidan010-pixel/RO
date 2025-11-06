#pragma once

namespace ITF::DX12
{
    // any VRAM handle is an element of a double linked list (thus inherit from VRAMHandleLinks)
    struct VRAMHandleLinks
    {
        mutable const VRAMHandleLinks * next;
        mutable const VRAMHandleLinks * prev;
        bool isValid() const { return next!=0 || prev!=0; }
    };
}
