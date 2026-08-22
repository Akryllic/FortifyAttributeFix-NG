#include "PCH.h"
#include "Settings.h"

#include <charconv>
#include <format>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "IniText.h"
#include "logger.h"

namespace
{
    constexpr auto INI_PATH = "Data/SKSE/Plugins/FortifyAttributeFixNG.ini";

    std::unordered_map<std::string, float> g_settings;

    using IniText::StripComment;
    using IniText::ToLower;
    using IniText::Trim;

    void WriteKey(std::string_view a_section, std::string_view a_key, const std::string& a_formatted)
    {
        const auto wantedSection = ToLower(std::string(a_section));
        const auto wantedKey = ToLower(std::string(a_key));

        std::vector<std::string> lines;
        {
            std::ifstream in(INI_PATH);
            std::string line;
            while (std::getline(in, line)) {
                if (!line.empty() && line.back() == '\r') {
                    line.pop_back();
                }
                lines.push_back(std::move(line));
            }
        }

        std::string section;
        bool written = false;
        std::size_t sectionEnd = 0;
        bool sawSection = false;

        for (std::size_t i = 0; i < lines.size() && !written; ++i) {
            const auto bare = StripComment(lines[i]);
            if (!bare.empty() && bare.front() == '[' && bare.back() == ']') {
                section = ToLower(Trim(bare.substr(1, bare.size() - 2)));
                if (section == wantedSection) {
                    sawSection = true;
                    sectionEnd = i + 1;
                }
                continue;
            }
            if (section != wantedSection) {
                continue;
            }
            sectionEnd = i + 1;

            const auto eq = bare.find('=');
            if (eq != std::string::npos && ToLower(Trim(bare.substr(0, eq))) == wantedKey) {
                lines[i] = a_formatted;
                written = true;
            }
        }

        if (!written) {
            if (sawSection) {
                lines.insert(lines.begin() + static_cast<std::ptrdiff_t>(sectionEnd), a_formatted);
            } else {
                lines.emplace_back();
                lines.push_back(std::format("[{}]", a_section));
                lines.push_back(a_formatted);
            }
        }

        std::ofstream out(INI_PATH, std::ios::trunc);
        if (!out.is_open()) {
            logger::error("Could not write [{}] {} to {}", a_section, a_key, INI_PATH);
            return;
        }
        for (const auto& line : lines) {
            out << line << '\n';
        }
        logger::info("Saved [{}] {} to {}", a_section, a_formatted, INI_PATH);
    }
}

namespace Settings
{
    void Load()
    {
        g_settings.clear();

        std::ifstream in(INI_PATH);
        if (!in.is_open()) {
            logger::warn("No {}, every setting falls back to its default.", INI_PATH);
            return;
        }

        std::string line;
        std::string section;
        while (std::getline(in, line)) {
            const auto bare = Trim(StripComment(line));
            if (bare.empty()) {
                continue;
            }
            if (bare.front() == '[' && bare.back() == ']') {
                section = ToLower(Trim(bare.substr(1, bare.size() - 2)));
                continue;
            }
            if (section != "settings") {
                continue;
            }

            const auto eq = bare.find('=');
            if (eq == std::string::npos) {
                continue;
            }

            const auto key = ToLower(Trim(bare.substr(0, eq)));
            const auto text = Trim(bare.substr(eq + 1));
            try {
                g_settings[key] = std::stof(text);
            } catch (const std::exception&) {
                logger::warn("{} is not a number, ignoring {} in {}", text, key, INI_PATH);
            }
        }

        logger::info("Loaded {} setting(s) from {}", g_settings.size(), INI_PATH);
    }

    float GetSetting(std::string_view a_key, float a_fallback)
    {
        const auto it = g_settings.find(ToLower(std::string(a_key)));
        return it != g_settings.end() ? it->second : a_fallback;
    }

    void SaveSetting(std::string_view a_key, float a_value)
    {
        g_settings[ToLower(std::string(a_key))] = a_value;
        WriteKey("Settings", a_key, std::format("{} = {}", a_key, a_value));
    }
}
