#pragma once

#include "Wildlands/Core/Base.h"

namespace Wildlands
{
	class RenderContext
	{
	public:
		static Unique<RenderContext> Create(void* window);
		virtual ~RenderContext() = default;

		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
	};
}
