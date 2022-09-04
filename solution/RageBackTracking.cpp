#include "Hooks.h"
#include "RageBackTracking.h"
#include "Ragebot.h"
#include "Resolver.h"
#include "AntiAims.h"
CMBacktracking* g_Backtrack = new CMBacktracking();

static std::deque<CIncomingSequence>sequences;
static int lastincomingsequencenumber;
int Real_m_nInSequencenumber;

void CMBacktracking::UpdateIncomingSequences()
{
    if (csgo->client_state)
    {
        if (csgo->client_state->pNetChannel)
        {
            if (csgo->client_state->pNetChannel->iInSequenceNr > lastincomingsequencenumber)
            {
                lastincomingsequencenumber = csgo->client_state->pNetChannel->iInSequenceNr;
                sequences.push_front(CIncomingSequence(csgo->client_state->pNetChannel->iInReliableState, csgo->client_state->pNetChannel->iOutReliableState,
                    csgo->client_state->pNetChannel->iInSequenceNr, interfaces.global_vars->realtime));
            }

            if (sequences.size() > 2048)
                sequences.pop_back();
        }
    }
}

void CMBacktracking::ClearIncomingSequences()
{
    sequences.clear();
}

void CMBacktracking::AddLatencyToNetchan(INetChannel* netchan, float Latency)
{
    for (auto& seq : sequences)
    {
        if (interfaces.global_vars->realtime - seq.curtime >= Latency
            || interfaces.global_vars->realtime - seq.curtime > 1)
        {
            netchan->iInReliableState = seq.inreliablestate;
            netchan->iInSequenceNr = seq.sequencenr;
            break;
        }
    }
}