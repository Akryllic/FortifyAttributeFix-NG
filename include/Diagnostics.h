#pragma once

namespace Diagnostics
{
    /// @brief Register the Debug page. Compiled to nothing unless FF_DEBUG_PANEL is defined.
    /// @note The release build has no debug UI in it at all, rather than a hidden one.
    void Register();

    /// @brief "debug" or "release", for the startup log line.
    [[nodiscard]] const char* BuildName();
}
