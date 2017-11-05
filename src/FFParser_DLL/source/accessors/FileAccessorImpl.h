#pragma once

#include <string>
#include <array>
#include <vector>
#include <utility>

#include "IFileAccessor.h"


namespace FFParser {

	class FileAccessorImpl : public IFileAccessor
	{
	private:
		std::array<std::string, size_t(EResourcePaths::NUMBER_OF_PATHS)> _resource_paths;
		std::vector<std::pair<std::string, std::string>> _profile_list;

		//methods
		std::string getFirefoxInstallDir() const;
		void setupPaths();
		void loadProfiles();

	public:
		FileAccessorImpl();
		virtual ~FileAccessorImpl() = default;

		//Interface methods
		virtual std::string getPathToResource(EResourcePaths resource, size_t profile = 0) const override;
		virtual size_t getFilesCount(const std::string& dir_path) const override;
		virtual void getFileList(std::vector<std::string>& list, const std::string& dir_path) const override;
		virtual void getProfiles(std::vector<std::string>& list) const override;
	};

}