#pragma once
#include <string>

namespace Wildlands
{
	class FileDialogs
	{
	public:
		/// <returns> return empty string mean cancelled </returns>
		static std::string OpenFile(const char* filter);
		/// <returns> return empty string mean cancelled </returns>
		static std::string SaveFile(const char* filter);
	};
}
