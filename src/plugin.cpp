#include "PCH.h"

#include "Fortify.h"
#include "Menu.h"
#include "Settings.h"
#include "logger.h"

namespace
{
#ifdef FF_DEBUG_PANEL
    constexpr auto BUILD_NAME = "debug";
#else
    constexpr auto BUILD_NAME = "release";
#endif

    void OnMessage(SKSE::MessagingInterface::Message* a_message)
    {
        if (!a_message)
            return;

        switch (a_message->type) {
        case SKSE::MessagingInterface::kDataLoaded:
            Settings::Load();
            Fortify::LoadSettings();
            Fortify::Install();
            Menu::Register();
            break;

        case SKSE::MessagingInterface::kPostLoadGame:
        case SKSE::MessagingInterface::kNewGame:
            Settings::Load();
            Fortify::LoadSettings();
            Fortify::OnGameLoaded(a_message->type == SKSE::MessagingInterface::kPostLoadGame);
            break;

        default:
            break;
        }
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
    SetupLog();
    logger::info("Plugin loaded, {} build", BUILD_NAME);

    // Sized once for every write_call, since a later request is ignored. Logging stays with SetupLog, which Init would otherwise replace. ItzIvy05/GoToBed/src/plugin.cpp:15
    SKSE::Init(a_skse, { .log = false, .trampoline = true, .trampolineSize = 1 << 7 });

    const auto messaging = SKSE::GetMessagingInterface();
    if (!messaging || !messaging->RegisterListener(OnMessage)) {
        logger::critical("Failed to register SKSE messaging listener.");
        return false;
    }

    return true;
}
