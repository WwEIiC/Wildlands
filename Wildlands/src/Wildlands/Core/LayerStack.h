#pragma once
#include "Wildlands/Core/Core.h"
#include "Wildlands/Events/Event.h"
#include "Wildlands/Core/Layer.h"

namespace Wildlands
{
	class WL_API LayerStack
	{
	public:
		LayerStack() = default;
		~LayerStack();

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		void PushOverlay(Layer* overlay);
		void PopOverlay(Layer* overlay);

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }

	private:
		/* layers will be stored in m_Layers like this:
		* Rendering order:	  --------->
		*		[layer_1, layer_2, ..., layer_n][olayer_1, olayer_2, ..., olayer_n]
		* Event handle order: <---------
		*/
		std::vector<Layer*> m_Layers;
		uint32_t m_LayerInsertIndex = 0;  //also the begin of overlay
	};
}

