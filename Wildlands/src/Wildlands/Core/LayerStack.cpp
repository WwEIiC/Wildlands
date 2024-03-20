#include "WLPCH.h"
#include "LayerStack.h"

namespace Wildlands
{
	LayerStack::~LayerStack()
	{
		WL_PROFILE_FUNCTION();
		for (Layer* layer : m_Layers)
		{
			layer->Detach();
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
		++m_LayerInsertIndex;
	}
	void LayerStack::PopLayer(Layer* layer)
	{
		std::vector<Layer*>::iterator target = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
		if (target != m_Layers.begin() + m_LayerInsertIndex)
		{
			m_Layers.erase(target);
			--m_LayerInsertIndex;
		}
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		m_Layers.emplace_back(overlay);
	}
	void LayerStack::PopOverlay(Layer* overlay)
	{
		std::vector<Layer*>::iterator target = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay);
		if (target != m_Layers.end())
		{
			m_Layers.erase(target);
		}
	}
}
