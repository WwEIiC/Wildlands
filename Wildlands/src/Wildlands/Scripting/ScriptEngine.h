#pragma once

namespace Wildlands
{
	class ScriptEngine
	{
	public:
		static void Init();
		static void Close();

	private:
		static void InitMono();
		static void CloseMono();
	};
}
