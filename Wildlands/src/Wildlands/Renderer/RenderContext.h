#pragma once

namespace Wildlands
{
	class RenderContext
	{
	public:
		static Unique<RenderContext> Create(void* window);

		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
	};
}
