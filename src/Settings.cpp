#include "Settings.h"

Settings* Settings::GetSingleton()
{
	static Settings singleton;
	return std::addressof(singleton);
}

void Settings::Load()
{
	constexpr auto path = L"Data/SKSE/Plugins/StableHands.ini";

	CSimpleIniA ini;
	ini.SetUnicode();

	ini.LoadFile(path);

	//TWEAKS
	tweaks.Load(ini);

	ini.SaveFile(path);
}

void Settings::Tweaks::Load(CSimpleIniA& a_ini)
{
	const char* section = "Tweaks";

	detail::get_value(a_ini, DPI, section, "fDPI", ";DPI for the menu mouse. This is relative to the vanilla DPI (ex: 0.5 = 50% DPI of the vanilla mouse)");
	detail::get_value(a_ini, deadzone, section, "fDeadzone", ";Ignore any mouse updates that would move the mouse less than the deadzone amount from its current position (recommend between 0-10 for best results)");

	logger::info("Loaded settings");
	logger::info("DPI = {}", DPI);
	logger::info("deadzone = {}", deadzone);
}
