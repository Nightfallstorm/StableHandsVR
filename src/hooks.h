#pragma once
#include "MouseCoords.h"
#include "SmoothingAlgorithms.h"
#include "Settings.h"

namespace hooks
{
	// Shizof's smoothing algorithm
	static inline smoothingalgorithm::SmoothingAlgorithm* smoother = new smoothingalgorithm::AveragingAlgorithm();

	struct MouseCoordsUpdateHook {
		// Note: a_x and a_y are not capped due to our hook logic removing the cap code, reapply the cap here with our DPI modifications
		static void thunk(MouseCoords* a_coords, float a_x, float a_y) {
		
			// Apply DPI
			auto DPI = Settings::GetSingleton()->tweaks.DPI;
			auto lengthDiff = (a_coords->maxX / DPI) - a_coords->maxX;
			a_x += lengthDiff / 2.0f;
			a_x = a_x * DPI;
			a_y += lengthDiff / 2.0f;
			a_y = a_y * DPI;

			// Apply smoothing			
			std::pair newCoords = smoother->SmoothCoords(a_coords, a_x, a_y);

			// Check deadzone
			auto deadzone = Settings::GetSingleton()->tweaks.deadzone;
			if (std::abs(a_x - a_coords->x) <= deadzone) {
				newCoords.first = a_coords->x;
			}
			if (std::abs(a_y - a_coords->y) <= deadzone) {
				newCoords.second = a_coords->y;
			}

			return func(a_coords, newCoords.first, newCoords.second);
		}

		static inline REL::Relocation<decltype(thunk)> func;

		static void Install() {
			REL::Relocation<std::uintptr_t> target{ REL::Offset(0x6B8320) };
			std::uintptr_t function = target.address() + 0x3E4;

			// Remove bounds from the original function. We will do bounds after calculating DPI
			std::uintptr_t XpercentCapStart = target.address() + 0x399;
			std::uintptr_t XpercentCapEnd = target.address() + 0x3AD;
			REL::safe_fill(XpercentCapStart, REL::NOP, XpercentCapEnd - XpercentCapStart);
			std::uintptr_t YpercentCapStart = target.address() + 0x3B5;
			std::uintptr_t YpercentCapEnd = target.address() + 0x3C9;
			REL::safe_fill(YpercentCapStart, REL::NOP, YpercentCapEnd - YpercentCapStart);

			// Hook mouse coords update function
			pstl::write_thunk_call<MouseCoordsUpdateHook>(function);
		}
	};

	struct RemoveCoordsCap {
		static void Install() {
			REL::Relocation<std::uintptr_t> target{ REL::Offset(0xF33CD0) };
			std::uintptr_t function = target.address() + 0x3E4;

			// Remove bounds from the original function. We will do bounds after calculating DPI
			std::uintptr_t XpercentCapStart = target.address() + 0x399;
			std::uintptr_t XpercentCapEnd = target.address() + 0x3AD;
			REL::safe_fill(XpercentCapStart, REL::NOP, XpercentCapEnd - XpercentCapStart);
			std::uintptr_t YpercentCapStart = target.address() + 0x3B5;
			std::uintptr_t YpercentCapEnd = target.address() + 0x3C9;
			REL::safe_fill(YpercentCapStart, REL::NOP, YpercentCapEnd - YpercentCapStart);

			// Hook mouse coords update function
			pstl::write_thunk_call<MouseCoordsUpdateHook>(function);
		}
	};
	void Install()
	{
		MouseCoordsUpdateHook::Install();
	}
}
