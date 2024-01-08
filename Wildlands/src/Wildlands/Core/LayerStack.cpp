#include "WLPCH.h"
#include "LayerStack.h"

namespace Wildlands
{
	LayerStack::~LayerStack()
	{
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
		layer->Attach();
	}
	void LayerStack::PopLayer(Layer* layer)
	{
		std::vector<Layer*>::iterator target = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
		if (target != m_Layers.begin() + m_LayerInsertIndex)
		{
			layer->Detach();
			m_Layers.erase(target);
			--m_LayerInsertIndex;
		}
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		m_Layers.emplace_back(overlay);
		overlay->Attach();
	}
	void LayerStack::PopOverlay(Layer* overlay)
	{
		std::vector<Layer*>::iterator target = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay);
		if (target != m_Layers.end())
		{
			overlay->Detach();
			m_Layers.erase(target);
		}
	}
}
