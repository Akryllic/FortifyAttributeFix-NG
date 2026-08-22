#pragma once

/// Plain INI settings. 
/// This is the default storage for anything a modlist author should be able to set once for every character. 
/// A GlobalVariable is only needed when a record CONDITION reads the value; per-playthrough state with no record reader belongs in the SKSE co-save instead.

namespace Settings
{
    /// @brief Read the INI. Call on kDataLoaded, and again whenever the file should be re-read.
    void Load();

    /// @brief One setting, or @p a_fallback when the key is absent.
    [[nodiscard]] float GetSetting(std::string_view a_key, float a_fallback);

    /// @brief Write one setting back to the INI, preserving comments and key order.
    void SaveSetting(std::string_view a_key, float a_value);
}
