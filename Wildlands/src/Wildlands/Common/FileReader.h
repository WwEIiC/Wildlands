#pragma once
#include "Singleton.h"

#include <string>

namespace Wildlands
{
	class FileReader : public Singleton<FileReader>
	{
		SINGLETON(FileReader)
	public:
		std::string Read(const char* filePath);
	};
}
