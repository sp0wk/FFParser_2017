#pragma once

#include <vector>
#include <string>


namespace FFParser {

	enum class EResourcePaths
	{
		INSTALLDIR = 0,
		PROFILES_INI,
		PROFILES,
		PROFILESLOCAL,
		PROFILEPATH,
		CACHE,
		DATABASE,
		LOGINS,
		NUMBER_OF_PATHS
	};


	class IFileAccessor
	{
	public:
		virtual std::string getPathToResource(EResourcePaths resource, size_t profile = 0) const = 0;
		virtual size_t getFilesCount(const std::string& path) const = 0;
		virtual void getFileList(std::vector<std::string>& list, const std::string& path) const = 0;
		virtual void getProfiles(std::vector<std::string>& list) const = 0;
		virtual size_t getNumberOfProfiles() const = 0;
		virtual ~IFileAccessor() {}
	};

}