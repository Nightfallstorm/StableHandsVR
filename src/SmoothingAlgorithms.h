#pragma once
#include "MouseCoords.h"
#include "Settings.h"

namespace smoothingalgorithm
{
	class SmoothingAlgorithm {
	public:
		virtual std::pair<float, float> SmoothCoords(MouseCoords* a_coords, float a_x, float a_y) = 0;
	};

	class AveragingAlgorithm: public SmoothingAlgorithm {
		// Higher = smoother/slower
		static constexpr std::uint32_t accuracy = 8;

		static_assert(accuracy > 1);

		static inline float x[accuracy] = { 0 };
		static inline float y[accuracy] = { 0 };

		virtual std::pair<float, float> SmoothCoords(MouseCoords* a_coords, float a_x, float a_y) override
		{
			UpdateCoordHistory(a_x, a_y);
			float newX = 0;
			float newY = 0;
			for (int i = 0; i < accuracy; i++) {
				newX += x[i];
				newY += y[i];
			}
			newX = newX != 0 ? (newX / (float)accuracy) : 0;
			newY = newY != 0 ? (newY / (float)accuracy) : 0;

			return { newX, newY };
		}

		static void UpdateCoordHistory(float a_newX, float a_newY)
		{
			for (int i = accuracy - 1; i > 0; i--) {
				x[i] = x[i - 1];
				y[i] = y[i - 1];
			}
			x[0] = a_newX;
			y[0] = a_newY;
		}
	};


	// Credit to Shizof
	class LowPassFilter: public SmoothingAlgorithm {
		float smoothedX;
		float smoothedY;

		bool timerInitialized = false;
		// These keep track of the time elapsed since the last frame and debug message timing
		// Outside parameters
		LARGE_INTEGER m_hpcFrequency;
		LARGE_INTEGER m_prevTime;
		double m_frameTime = 0;

		virtual std::pair<float, float> SmoothCoords(MouseCoords* a_coords, float a_x, float a_y) {
			if (!timerInitialized) {
				timerInitialized = true;
				//inside a start function:
				// Initialize counter frequency and initial time
				QueryPerformanceFrequency(&m_hpcFrequency);
				QueryPerformanceCounter(&m_prevTime);
			}

			//inside hook func:
			LARGE_INTEGER newTime;
			QueryPerformanceCounter(&newTime);
			m_frameTime = (double)(newTime.QuadPart - m_prevTime.QuadPart) / m_hpcFrequency.QuadPart;
			if (m_frameTime > 0.05f) {
				m_frameTime = 0.05f;
			}
			m_prevTime = newTime;

			bool shouldSmooth = true;
			if (fabsf(a_x - smoothedX) > 300.0f || a_x <= a_coords->minX || a_x >= a_coords->maxX) {
				smoothedX = a_x;
				shouldSmooth = false;
			}
			if (fabsf(a_y - smoothedY) > 300.0f || a_y <= a_coords->minY || a_y >= a_coords->maxY) {
				smoothedY = a_y;
				shouldSmooth = false;
			}

			if (shouldSmooth) {
				smoothedX = Calculate(a_x, smoothedX);
				smoothedY = Calculate(a_y, smoothedY);
			}		

			return { smoothedX, smoothedY };
		}

		float Calculate(float a_coord, float a_smoothedCoord)
		{
			float absVal = abs(a_coord - a_smoothedCoord);
			if (absVal < 0.1f) {
				absVal = 0.1f;
			}

			return a_smoothedCoord + m_frameTime * ((a_coord - a_smoothedCoord) / (2.0f * (1.0f / absVal) * 0.2f));
		}
	};
}
