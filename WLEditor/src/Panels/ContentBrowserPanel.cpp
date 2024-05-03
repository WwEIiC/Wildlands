#include "ContentBrowserPanel.h"

#include "Wildlands/Project/Project.h"

#include <imgui.h>

namespace Wildlands
{
	ContentBrowserPanel::ContentBrowserPanel()
		:m_BaseDirectory(Project::GetAssetDirectory()), m_CurrentDirectory(m_BaseDirectory)
	{
		m_FileFolderIcon = Texture2D::Create("resources/icons/ContentBrowser/FileFolderIcon.png");
		m_FileIcon = Texture2D::Create("resources/icons/ContentBrowser/FileIcon.png");
	}
	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		if (m_CurrentDirectory != std::filesystem::path(m_BaseDirectory))
		{
			if (ImGui::Button("<--"))
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
		}

		static float padding = 16.0f;
		static float thumbnailSize = 100.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const std::filesystem::path& path = directoryEntry.path();
			std::string filenameString = path.filename().string();

			// because every filefolder/file buttons have the same icon(will be the same id).
			// we need to indentify them by their name.
			ImGui::PushID(filenameString.c_str());

			Ref<Texture2D> icon = directoryEntry.is_directory() ? m_FileFolderIcon : m_FileIcon;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
			ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
			if (ImGui::BeginDragDropSource())
			{
				std::filesystem::path relativePath(path);
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}
			ImGui::PopStyleColor();

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					m_CurrentDirectory /= directoryEntry.path().filename();
			}
			ImGui::TextWrapped(filenameString.c_str());

			ImGui::NextColumn();
			ImGui::PopID();
		}

		ImGui::Columns(1);

		ImGui::End();
	}
}
