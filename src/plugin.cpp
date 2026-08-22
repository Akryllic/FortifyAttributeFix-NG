#include "PCH.h"

#include "Diagnostics.h"
#include "Fortify.h"
#include "MCP.h"
#include "Settings.h"
#include "logger.h"

namespace
{
    void OnMessage(SKSE::MessagingInterface::Message* a_message)
    {
        if (!a_message)
            return;

        switch (a_message->type) {
        case SKSE::MessagingInterface::kDataLoaded:
            Settings::Load();
            Fortify::LoadSettings();
            Fortify::Install();
            MCP::Register();
            Diagnostics::Register();
            break;

        case SKSE::MessagingInterface::kPostLoadGame:
        case SKSE::MessagingInterface::kNewGame:
            Settings::Load();
            Fortify::LoadSettings();
            Fortify::OnGameLoaded();
            break;

        default:
            break;
        }
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
    SetupLog();
    logger::info("Plugin loaded, {} build", Diagnostics::BuildName());

    SKSE::Init(a_skse);

    const auto messaging = SKSE::GetMessagingInterface();
    if (!messaging || !messaging->RegisterListener(OnMessage)) {
        logger::critical("Failed to register SKSE messaging listener.");
        return false;
    }

    return true;
}
