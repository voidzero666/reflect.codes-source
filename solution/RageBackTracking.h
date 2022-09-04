#pragma once
struct CIncomingSequence
{
    CIncomingSequence(int instate22, int outstate22, int seqnr22, float ctime22)
    {
        inreliablestate = instate22;
        outreliablestate = outstate22;
        sequencenr = seqnr22;
        curtime = ctime22;
    }
    int inreliablestate;
    int outreliablestate;
    int sequencenr;
    float curtime;
};
class CMBacktracking {
public:
    void UpdateIncomingSequences();
    void ClearIncomingSequences();
    void AddLatencyToNetchan(INetChannel* netchan, float Latency);
};

extern CMBacktracking* g_Backtrack;