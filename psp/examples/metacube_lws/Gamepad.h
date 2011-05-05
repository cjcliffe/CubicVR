class Gamepad
{
	public:
		Gamepad();
		~Gamepad();
		
		bool Poll();
		
		unsigned int Buttons();
		bool ButtonDown(unsigned int buttons);
		bool ButtonUp(unsigned int buttons);
		
		float AnalogX();
		float AnalogY();
		
	private:
		SceCtrlData pad;
		SceCtrlData lastpad;	
};
