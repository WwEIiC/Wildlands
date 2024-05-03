#include "WLPCH.h"
#include "FileSystem.h"
#include "Wildlands/Core/Log.h"
#include <filesystem>

namespace Wildlands
{
	Buffer FileSystem::ReadFileBinary(const std::filesystem::path& filepath)
	{
		// move the pointer to the end of the file first.
		std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

		if (!stream)
		{
			WL_CORE_ERROR("FileSystem::Failed to open file '{0}'", filepath.string());
			return {};
		}

		std::streampos end = stream.tellg();
		stream.seekg(0, std::ios::beg); // move back to the begin of the file.
		uint64_t size = end - stream.tellg();

		if (size == 0)
		{
			WL_CORE_TRACE("FileSystem::File '{0}' is empty", filepath.string());
			return {};
		}

		Buffer buffer(size);
		stream.read(buffer.As<char>(), size);
		stream.close();
		return buffer;
	}
}
