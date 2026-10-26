#pragma once

/// The fix itself.
/// Skyrim stores a current attribute as base + permanent + temporary + damage, where damage is a negative number.
/// A fortify raises a modifier, the permanent one on the player and the temporary one on an NPC, so it raises current as well as maximum.
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

    /// @brief Clear the per-session state, and after a save loads re-read Survival Mode Improved's penalties. Call on kPostLoadGame and kNewGame.
    void OnGameLoaded(bool a_fromSave);

    [[nodiscard]] Mode CurrentMode();
    void SetMode(Mode a_mode);

    [[nodiscard]] bool PlayerOnly();
    void SetPlayerOnly(bool a_value);

    /// @brief Write the running totals to the log. Called on every load and every mode change.
    void LogStats();
}
