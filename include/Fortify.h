#pragma once

#include <cstdint>
#include <vector>

/// The fix itself.
/// Skyrim stores a current attribute as base + permanent + temporary + damage, where damage is a negative number.
/// A fortify raises the temporary modifier, so it raises current as well as maximum.
/// When it expires the maximum drops but the damage modifier does not move, and if accumulated damage now exceeds the maximum the current value computes below zero and the actor dies.
namespace Fortify
{
    enum class Mode
    {
        /// fortify never moves the current value in either direction.
        kAbsolute = 0,
        /// fortify keeps the current value at the same fraction of the maximum.
        kRatio = 1,
        /// Do nothing. For proving the bug is real before proving the fix works.
        kOff = 2,
        /// fortify tops the value up to its new maximum. Losing one still never moves it.
        kRestore = 3,
    };

    /// @brief Hook every call site of the engine's actor value modifier function. Call on kDataLoaded.
    void Install();

    /// @brief Re-read the mode and the actor scope from the INI.
    void LoadSettings();

    /// @brief Clear the per-session state. Call on kPostLoadGame and kNewGame.
    void OnGameLoaded();

    [[nodiscard]] Mode CurrentMode();
    void SetMode(Mode a_mode);

    [[nodiscard]] bool PlayerOnly();
    void SetPlayerOnly(bool a_value);

    /// @brief Write the running totals to the log. Called on every load and every mode change.
    void LogStats();

    struct Stats
    {
        std::uint64_t seen{ 0 };
        std::uint64_t corrected{ 0 };
        std::uint64_t restoresLeftAlone{ 0 };
        std::uint64_t deathsPrevented{ 0 };
    };

    [[nodiscard]] Stats GetStats();

    struct CallSite
    {
        /// SkyrimSE.exe+offset, for logs and the panel.
        std::uintptr_t offset{ 0 };
        std::uintptr_t address{ 0 };
        /// One of the five the original mod publishes.
        bool published{ false };
        bool hooked{ false };
    };

    /// @brief Every call to the modifier function found in .text, and whether we hook it.
    /// @note Scanned once during Install, before the hooks go in, because write_call rewrites the displacements it would read. The retail exe cannot be scanned on disk; Steam's DRM leaves .text encrypted there.
    [[nodiscard]] std::vector<CallSite> GetCallSites();

    /// @brief Log the player's full attribute state on the next @p a_samples modifier changes.
    /// @note Rides the traffic already passing through the hooks, so it needs no tick of its own.
    void StartStateLog(std::int32_t a_samples);
}
