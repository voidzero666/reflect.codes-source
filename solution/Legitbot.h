
class Legitbot : public Singleton<Legitbot>
{
public:
	animation* GetAimAnimation();
	void Backtrack();
	void AutoPistol();
	void Run(CUserCmd* cmd);
	Vector calculateRelativeAngle(const Vector& source, const Vector& destination, const Vector& viewAngles);
	void Triggerbot(CUserCmd* cmd);
};