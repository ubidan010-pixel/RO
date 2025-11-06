#pragma once

// Helpers for boolean logic operations

namespace ITF
{
    // Logical implication, imply(a, b): a implies b
    inline bool imply(bool a, bool b) { return !a || b; } // a => b

} // namespace ITF

