#pragma once
#include "Hooks.h"
#include "iBaseClientDLL.h"
#include "EnginePrediction.h"
#include "Dormant.h"

void __fastcall Hooked_PacketStart(void* ecx, void* edx, int incoming, int outgoing) {
   // static auto original_fn = g_pClientStateAdd->GetOriginal<fnPacketStart>(g_HookIndices[fnva1(hs::Hooked_PacketStart.s().c_str())]);

    if (csgo->DoUnload)
        return g_pClientStateAdd->callOriginal<void, 5>(incoming, outgoing);

    if (!csgo->local->isAlive())
        return g_pClientStateAdd->callOriginal<void, 5>(incoming, outgoing);

    if (csgo->packets.empty())
        return g_pClientStateAdd->callOriginal<void, 5>(incoming, outgoing);

    for (auto it = csgo->packets.rbegin(); it != csgo->packets.rend(); ++it)
    {
        if (!it->is_outgoing)
            continue;

        if (it->cmd_number == outgoing || outgoing > it->cmd_number && (!it->is_used || it->previous_command_number == outgoing))
        {
            it->previous_command_number = outgoing;
            it->is_used = true;
            g_pClientStateAdd->callOriginal<void, 5>(incoming, outgoing);
            break;
        }
    }

    auto result = false;

    for (auto it = csgo->packets.begin(); it != csgo->packets.end();)
    {
        if (outgoing == it->cmd_number || outgoing == it->previous_command_number)
            result = true;

        if (outgoing > it->cmd_number && outgoing > it->previous_command_number)
            it = csgo->packets.erase(it);
        else
            ++it;
    }

    if (!result)
        g_pClientStateAdd->callOriginal<void, 5>(incoming, outgoing);
}

void __fastcall Hooked_PacketEnd(void* ecx, void* edx)
{
   // static auto original_fn = g_pClientStateAdd->GetOriginal <fnPacketEnd>(g_HookIndices[fnva1(hs::Hooked_PacketEnd.s().c_str())]);


    if (!csgo->local->isAlive())  //-V807
    {
        csgo->c_data.clear();
        return  g_pClientStateAdd->callOriginal<void, 6>();
    }
    auto clientstate = (CClientState*)ecx;
    if (*(int*)((uintptr_t)ecx + 0x164) == *(int*)((uintptr_t)ecx + 0x16C))
    {
        auto ack_cmd = *(int*)((uintptr_t)ecx + 0x4D2C);
        auto correct = std::find_if(csgo->c_data.begin(), csgo->c_data.end(),
            [&ack_cmd](const correction_data& other_data)
            {
                return other_data.command_number == ack_cmd;
            }
        );

        auto netchannel = interfaces.engine->GetNetChannelInfo();

        if (netchannel && correct != csgo->c_data.end())
        {
            if (csgo->velocity_modifier > csgo->local->GetVelocityModifier() + 0.1f)
            {
                auto weapon = csgo->weapon;

                if (!weapon || weapon->GetItemDefinitionIndex() != WEAPON_REVOLVER && !weapon->IsNade()) //-V648
                {
                    for (auto& number : csgo->choked_number)
                    {
                        auto cmd = &interfaces.input->m_pCommands[number % 150];
                        auto verified = &interfaces.input->m_pVerifiedCommands[number % 150];

                        if (cmd->buttons & (IN_ATTACK | IN_ATTACK2))
                        {
                            cmd->buttons &= ~IN_ATTACK;

                            verified->m_cmd = *cmd;
                            verified->m_crc = cmd->GetChecksum();
                        }
                    }
                }
            }

            csgo->velocity_modifier = csgo->local->GetVelocityModifier();
        }
    }

    return  g_pClientStateAdd->callOriginal<void, 6>();
}


int __fastcall Hooked_SendDatagram(void* netchan, void*, void* datagram)
{
   // static auto SendDatagram = csgo->g_pNetChannelHook->GetOriginal<send_datagram_fn>(g_HookIndices[fnva1(hs::Hooked_SendDatagram.s().c_str())]);

    return csgo->g_pNetChannelHook->callOriginal<int, 46>(netchan, datagram);


}
/*

void __fastcall Hooked_ProcessPacket(void* ecx, void* edx, void* packet, bool header) {

    static auto ProcessPacket = csgo->g_pNetChannelHook->GetOriginal<ProcessPacket_t>(g_HookIndices[fnva1(hs::Hooked_ProcessPacket.s().c_str())]);
    if (!csgo->client_state->pNetChannel)
        return ProcessPacket(ecx, packet, header);

    ProcessPacket(ecx, packet, header);

    // get this from CL_FireEvents string "Failed to execute event for classId" in engine.dll
    for (CEventInfo* it{ csgo->client_state->pEvents }; it != nullptr; it = it->m_next) {
        if (!it->m_class_id)
            continue;

        // set all delays to instant.
        it->m_fire_delay = 0.f;
    }

    interfaces.engine->FireEvents();
}
*/