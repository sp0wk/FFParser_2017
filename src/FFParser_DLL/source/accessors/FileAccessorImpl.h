#pragma once

#include <string>
#include <array>
#include <vector>
#include <utility>

#include "IFileAccessor.h"


namespace FFParser {

	class FileAccessorImpl : public IFileAccessor
	{
	public:
		FileAccessorImpl();
		virtual ~FileAccessorImpl() = default;

		//Interface methods
		virtual std::string getPathToResource(EResourcePaths resource, size_t profile = 0) const override;
		virtual size_t getFilesCount(const std::string& dir_path) const override;
		virtual void getFileList(std::vector<std::string>& list, const std::string& dir_path) const override;
		virtual void getProfiles(std::vector<std::string>& list) const override;
		virtual size_t getNumberOfProfiles() const override;

	private:
		struct ProfileInfo
		{
			std::string name;
			std::string path;
			std::string cache_path;
		};

		std::array<std::string, size_t(EResourcePaths::NUMBER_OF_PATHS)> _resource_paths;
		std::vector<ProfileInfo> _profile_list;

		//methods
		std::string getFirefoxInstallDir() const;
		void setupPaths();
		void loadProfiles();
	};

}