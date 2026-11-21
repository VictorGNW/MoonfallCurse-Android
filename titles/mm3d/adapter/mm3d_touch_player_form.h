#pragma once

#include "mm3d_title_contracts.h"
#include "../restoration/enums.h"
#include "../restoration/layout.h"

namespace Oot3dNativeGame {

// Read the live actor so the art follows the applied form, including after a
// save-state restore, rather than the equipped mask or requested transformation.
template <class Memory>
bool Mm3dPlayerIsZora(Memory& memory) {
    using namespace Mm3dRestoration;
    constexpr auto playType = static_cast<uint8_t>(Enum::State::StateType::Play);
    constexpr auto playerCategory = static_cast<uint8_t>(Enum::Actor::Type::Player);
    const auto inHeap = [](uint32_t address, uint32_t bytes) {
        return address >= 0x08000000 && address <= 0x40000000 - bytes && (address & 3U) == 0;
    };

    uint32_t view = 0;
    if (!memory.Read32(kMm3dCurrentViewPointer, &view) || view < kMm3dPlayViewOffset ||
        !inHeap(view - kMm3dPlayViewOffset, Layout::GlobalContext::Size))
        return false;

    const uint32_t play = view - kMm3dPlayViewOffset;
    const uint32_t playerList = play + Layout::GlobalContext::actors__lists +
                                playerCategory * Layout::ActorList::Size;
    uint8_t state = 0, form = 0;
    uint16_t actor = 0;
    uint32_t player = 0;
    return memory.Read8(play + Layout::GlobalContext::type, &state) && state == playType &&
           memory.Read32(playerList + Layout::ActorList::first, &player) &&
           inHeap(player, Layout::act_Player::active_form + 1) &&
           memory.Read16(player + Layout::act_Actor::id, &actor) &&
           actor == static_cast<uint16_t>(Enum::Actor::Id::Player) &&
           memory.Read8(player + Layout::act_Player::active_form, &form) &&
           form == static_cast<uint8_t>(Enum::Player::Form::Zora);
}

} // namespace Oot3dNativeGame
