#pragma once

namespace Util
{
    [[nodiscard]] inline RE::Actor* Player()
    {
        return RE::PlayerCharacter::GetSingleton();
    }

    // Name for logs. Falls back to the form id when a base name is missing
    [[nodiscard]] inline std::string Who(RE::Actor* a_actor)
    {
        if (!a_actor) return "none";
        if (const auto* base = a_actor->GetActorBase(); base && base->GetName()) {
            if (const std::string name = base->GetName(); !name.empty()) return name;
        }
        return std::format("{:08X}", a_actor->GetFormID());
    }
}
