#pragma once
#include <string>
#include <optional>

namespace Wildlands
{
	class FileDialogs
	{
	public:
		/// <returns> return empty string mean cancelled </returns>
		static std::optional<std::string> OpenFile(const char* filter);
		/// <returns> return empty string mean cancelled </returns>
		static std::optional<std::string> SaveFile(const char* filter);
	};
}
