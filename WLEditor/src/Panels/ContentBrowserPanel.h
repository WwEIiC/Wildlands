#pragma once
#include <filesystem>
#include "Wildlands/Renderer/Texture.h"

namespace Wildlands
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender();

	private:
		/// <summary> asset directory </summary>
		std::filesystem::path m_BaseDirectory;
		std::filesystem::path m_CurrentDirectory;

		Ref<Texture2D> m_FileFolderIcon;
		Ref<Texture2D> m_FileIcon;
	};
}

