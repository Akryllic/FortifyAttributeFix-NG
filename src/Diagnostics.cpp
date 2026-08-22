#include "PCH.h"
#include "Diagnostics.h"

#ifdef FF_DEBUG_PANEL

#include "Fortify.h"
#include "PanelUI.h"
#include "SKSEMCP/SKSEMenuFramework.hpp"
#include "Util.h"
#include "logger.h"

namespace
{
    void DrawLogging()
    {
        const auto level = spdlog::get_level();

        const auto levelButton = [&](const char* a_label, spdlog::level::level_enum a_level) {
            if (level == a_level) {
                ImGuiMCP::TextColored(PanelUI::GOOD, "%s", a_label);
            } else if (ImGuiMCP::Button(a_label)) {
                spdlog::set_level(a_level);
                spdlog::flush_on(a_level);
                logger::info("Log level set to {}.", a_label);
            }
        };

        ImGuiMCP::Text("Level");
        ImGuiMCP::SameLine();
        levelButton("info", spdlog::level::info);
        ImGuiMCP::SameLine();
        levelButton("debug", spdlog::level::debug);
        ImGuiMCP::SameLine();
        levelButton("trace", spdlog::level::trace);
        ImGuiMCP::TextDisabled("debug shows every ceiling change and every correction.");

        ImGuiMCP::Spacing();
        if (ImGuiMCP::Button("Log 200 state samples")) {
            Fortify::StartStateLog(200);
        }
        ImGuiMCP::TextDisabled("Press, close the menu, then stand still and let regeneration try.");
        ImGuiMCP::TextDisabled("Writes every modifier change on you, with the regen delay timers.");
    }

    void DrawStats()
    {
        const auto s = Fortify::GetStats();

        ImGuiMCP::SeparatorText("Since the game started");
        ImGuiMCP::Text("Modifier changes seen   %llu", s.seen);
        ImGuiMCP::Text("Corrected               %llu", s.corrected);
        ImGuiMCP::Text("Restores left alone     %llu", s.restoresLeftAlone);
        ImGuiMCP::TextColored(s.deathsPrevented > 0 ? PanelUI::GOOD : PanelUI::BAD, "Deaths prevented        %llu",
                              s.deathsPrevented);

        // A run with corrections but zero restores means the two are not being told apart, which would mean healing is being cancelled
        if (s.corrected > 0 && s.restoresLeftAlone == 0) {
            ImGuiMCP::TextColored(PanelUI::BAD, "No restores seen. Healing may be being cancelled.");
        }

        const auto sites = Fortify::GetCallSites();
        ImGuiMCP::SeparatorText("Call sites in the executable");
        if (sites.empty()) {
            ImGuiMCP::TextColored(PanelUI::BAD, "None found. The scan did not run.");
        } else {
            std::size_t hooked = 0;
            for (const auto& site : sites) {
                if (site.hooked) ++hooked;
            }
            ImGuiMCP::TextColored(hooked == sites.size() ? PanelUI::GOOD : PanelUI::BAD,
                                  "%llu found, %llu hooked", static_cast<unsigned long long>(sites.size()),
                                  static_cast<unsigned long long>(hooked));
            for (const auto& site : sites) {
                ImGuiMCP::TextColored(site.hooked ? PanelUI::GOOD : PanelUI::BAD, "  SkyrimSE.exe+%llX  %s%s",
                                      static_cast<unsigned long long>(site.offset),
                                      site.published ? "" : "not in the original",
                                      site.hooked ? "" : "   NOT HOOKED");
            }
        }
    }

    void DrawPlayer()
    {
        auto* player = Util::Player();
        if (!player) {
            return;
        }

        ImGuiMCP::SeparatorText("Player health");

        auto& mods = player->GetActorRuntimeData().healthModifiers;
        auto* owner = player->AsActorValueOwner();
        const float current = owner->GetActorValue(RE::ActorValue::kHealth);
        const float damage = mods.modifiers[RE::ACTOR_VALUE_MODIFIERS::kDamage];

        ImGuiMCP::Text("base       %.1f", owner->GetBaseActorValue(RE::ActorValue::kHealth));
        ImGuiMCP::Text("permanent  %.1f", mods.modifiers[RE::ACTOR_VALUE_MODIFIERS::kPermanent]);
        ImGuiMCP::Text("temporary  %.1f", mods.modifiers[RE::ACTOR_VALUE_MODIFIERS::kTemporary]);
        ImGuiMCP::Text("damage     %.1f", damage);
        ImGuiMCP::Text("current    %.1f of %.1f", current, current - damage);

        // This is the bug. Without the fix, the fortify expiring makes it true
        if (current - damage - mods.modifiers[RE::ACTOR_VALUE_MODIFIERS::kTemporary] <= 0.0f) {
            ImGuiMCP::TextColored(PanelUI::BAD, "A fortify lapsing right now would drop you to zero.");
        }

        ImGuiMCP::SeparatorText("Regeneration");

        // Three per attribute countdowns the engine holds regeneration off for (RE/H/HighProcessData.h:192-194)
        auto* process = player->GetActorRuntimeData().currentProcess;
        if (!process || !process->high) {
            ImGuiMCP::TextDisabled("no high process, the delays are not readable here");
            return;
        }

        const auto delay = [](const char* a_label, float a_value) {
            ImGuiMCP::TextColored(a_value > 0.0f ? PanelUI::BAD : PanelUI::GOOD, "%-9s %.2f", a_label, a_value);
        };

        delay("health", process->high->healthRegenDelay);
        delay("magicka", process->high->magickaRegenDelay);
        delay("stamina", process->high->staminaRegenDelay);
        ImGuiMCP::TextDisabled("Red means the engine is holding regeneration off for that attribute.");
        ImGuiMCP::TextDisabled("Pinned above zero while nothing is hitting you is the thing to catch.");
    }

    void __stdcall RenderDebug()
    {
        DrawLogging();
        DrawStats();
        DrawPlayer();

    }
}

namespace Diagnostics
{
    void Register()
    {
        if (!SKSEMenuFramework::IsInstalled()) {
            return;
        }

        SKSEMenuFramework::AddSectionItem("Debug", RenderDebug);
        logger::info("Diagnostics: Debug page registered.");
    }

    const char* BuildName()
    {
        return "debug";
    }
}

#else

namespace Diagnostics
{
    void Register() {}

    const char* BuildName()
    {
        return "release";
    }
}

#endif
