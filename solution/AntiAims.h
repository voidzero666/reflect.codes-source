#pragma once
#define TICK_INTERVAL            ( interfaces.global_vars->interval_per_tick )
#define TIME_TO_TICKS( dt )        ( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define TICKS_TO_TIME( t )        ( TICK_INTERVAL *( t ) )

class CMAntiAim : public Singleton<CMAntiAim>
{
private:
	void Yaw(bool legit_aa);
	void Pitch(bool legit_aa);
public:
	void Fakelag();
	void Sidemove();
	void Run();
	bool should_aa;
	bool can_exploit(int tickbase_shift);
};
extern bool CanDT();
extern bool CanHS();