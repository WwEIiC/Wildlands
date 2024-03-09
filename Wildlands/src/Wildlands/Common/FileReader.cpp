#include "WLPCH.h"
#include "FileReader.h"

#include <fstream>

namespace Wildlands
{
    std::string FileReader::Read(const char* filePath)
	{
        std::ifstream file(filePath);

        if (!file.is_open())
        {
            WL_CORE_ERROR("Failed to open file: {0}", filePath);
            return "";
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
	}
}