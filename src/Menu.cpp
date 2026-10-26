#include "PCH.h"
#include "Menu.h"

#include <iterator>

#include <FUCK_API.h>

#include "Fortify.h"
#include "Settings.h"
#include "logger.h"

namespace
{
    constexpr ImVec4 BAD{ 0.85f, 0.25f, 0.25f, 1.0f };

    class SettingsPage : public FUCK::ITool
    {
    public:
        const char* Name() const override
        {
            return "Settings";
        }

        const char* Group() const override
        {
            return "Fortify Attribute Fix - NG";
        }

        void Draw() override
        {
            FUCK::SeparatorText("Behaviour");

            int mode = static_cast<int>(Fortify::CurrentMode());
            const char* modes[] = { "Absolute", "Ratio", "Off", "Restore" };
            if (FUCK::Combo("Mode", &mode, modes, static_cast<int>(std::size(modes)))) {
                Fortify::SetMode(static_cast<Fortify::Mode>(mode));
                Settings::SaveSetting("Mode", static_cast<float>(mode));
            }
            FUCK::SetTooltip(
                "Applies to food, potions, spells and worn enchantments alike.\n\n"
                "Absolute: Fortify raises your maximum and never moves your current value.\n"
                "Ratio: Fortify keeps your current value at the same fraction of your maximum.\n"
                "Off: vanilla, including dying when a fortify health effect expires.\n"
                "Restore: Fortify fills you to its new maximum, and losing one still never hurts you.");

            switch (static_cast<Fortify::Mode>(mode)) {
            case Fortify::Mode::kAbsolute:
                FUCK::TextDisabled("A fortify raises your ceiling without handing you the health.");
                break;
            case Fortify::Mode::kRatio:
                FUCK::TextDisabled("A fortify gives health in proportion to how hurt you are.");
                break;
            case Fortify::Mode::kOff:
                FUCK::TextColored(BAD, "Nothing is being corrected.");
                break;
            case Fortify::Mode::kRestore:
                FUCK::TextDisabled("A fortify fills you up, and losing one costs you nothing.");
                break;
            }

            FUCK::SeparatorText("Scope");

            bool playerOnly = Fortify::PlayerOnly();
            if (FUCK::Checkbox("Player only", &playerOnly)) {
                Fortify::SetPlayerOnly(playerOnly);
                Settings::SaveSetting("PlayerOnly", playerOnly ? 1.0f : 0.0f);
            }
            FUCK::SetTooltip("NPCs hit the same bug. Leave this off unless you are chasing a conflict.");
        }
    };

    SettingsPage g_settingsPage;
}

namespace Menu
{
    void Register()
    {
        // Connected under the plugin's own name, then registered once, as ItzIvy05/Show-Player-In-Inventory/src/SettingsUI.cpp:311-317 does
        if (!FUCK::Connect(SKSE::GetPluginName().data())) {
            logger::info("Menu: FLICK not connected, no menu registered.");
            return;
        }

        FUCK::RegisterTool(&g_settingsPage);
        logger::info("Menu: registered the FLICK page.");
    }
}
