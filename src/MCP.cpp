#include "PCH.h"
#include "MCP.h"

#include <algorithm>
#include <iterator>

#include "Fortify.h"
#include "PanelUI.h"
#include "SKSEMCP/SKSEMenuFramework.hpp"
#include "Settings.h"
#include "logger.h"

namespace
{
    void __stdcall RenderSettings()
    {
        ImGuiMCP::SeparatorText("Behaviour");

        int mode = static_cast<int>(Fortify::CurrentMode());
        const char* modes[] = { "Absolute", "Ratio", "Off", "Restore" };
        ImGuiMCP::SetNextItemWidth(180.0f);
        if (ImGuiMCP::Combo("Mode", &mode, modes, static_cast<int>(std::size(modes)))) {
            Fortify::SetMode(static_cast<Fortify::Mode>(mode));
            Settings::SaveSetting("Mode", static_cast<float>(mode));
        }
        PanelUI::HelpMarker(
            "Applies to food, potions, spells and worn enchantments alike.\n\n"
            "Absolute: Fortify raises your maximum and never moves your current value.\n"
            "Ratio: Fortify keeps your current value at the same fraction of your maximum.\n"
            "Off: vanilla, including dying when a fortify health effect expires.\n"
            "Restore: Fortify fills you to its new maximum, and losing one still never hurts you.");

        switch (static_cast<Fortify::Mode>(mode)) {
        case Fortify::Mode::kAbsolute:
            ImGuiMCP::TextDisabled("A fortify raises your ceiling without handing you the health.");
            break;
        case Fortify::Mode::kRatio:
            ImGuiMCP::TextDisabled("A fortify gives health in proportion to how hurt you are.");
            break;
        case Fortify::Mode::kOff:
            ImGuiMCP::TextColored(PanelUI::BAD, "Nothing is being corrected.");
            break;
        case Fortify::Mode::kRestore:
            ImGuiMCP::TextDisabled("A fortify fills you up, and losing one costs you nothing.");
            break;
        }

        ImGuiMCP::SeparatorText("Scope");

        bool playerOnly = Fortify::PlayerOnly();
        if (ImGuiMCP::Checkbox("Player only", &playerOnly)) {
            Fortify::SetPlayerOnly(playerOnly);
            Settings::SaveSetting("PlayerOnly", playerOnly ? 1.0f : 0.0f);
        }
        PanelUI::HelpMarker("NPCs hit the same bug. Leave this off unless you are chasing a conflict.");
    }
}

namespace MCP
{
    void Register()
    {
        if (!SKSEMenuFramework::IsInstalled()) {
            logger::info("MCP: SKSE Menu Framework absent, no menu registered.");
            return;
        }

        SKSEMenuFramework::SetSection("Fortify Attribute Fix - NG");
        SKSEMenuFramework::AddSectionItem("Settings", RenderSettings);
        logger::info("MCP: registered SKSE Menu Framework section.");
    }
}
