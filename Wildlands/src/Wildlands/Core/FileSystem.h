#pragma once
#include "Wildlands/Core/Buffer.h"

namespace Wildlands
{
	class FileSystem
	{
	public:
		static Buffer ReadFileBinary(const std::filesystem::path& filepath);
	};
}

