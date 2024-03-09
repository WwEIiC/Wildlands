#pragma once

namespace Wildlands
{
	class Timestep
	{
	public:
		Timestep(float startTime)
			: m_Time(startTime)
		{
		}

		float GetSecondsTime() const { return m_Time; }
		float GetMillisecondsTime() const { return m_Time * 1000.0f; }

	public:
		operator float() const { return m_Time; }
	private:
		float m_Time;
	};
}

