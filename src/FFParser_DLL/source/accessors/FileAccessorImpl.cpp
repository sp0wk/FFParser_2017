#include "FileAccessorImpl.h"
#include "helpers/ErrorHandler.h"

#include <Windows.h>	//for registry API

#include <boost/filesystem.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>


using namespace boost::filesystem;


namespace FFParser {

	//ctor
	FileAccessorImpl::FileAccessorImpl()
	{
		setupPaths();
		loadProfiles();
	}


	//methods

	std::string FileAccessorImpl::getFirefoxInstallDir() const
	{
		const size_t BUFSIZE = 2048;
		char buf[BUFSIZE] {0};
		DWORD len = BUFSIZE;
		DWORD dwType = REG_SZ;
		HKEY hKey = 0;
		LSTATUS st = 0;
		DWORD key6432 = 0;

		std::string result;
		std::string subkey = "SOFTWARE\\Mozilla\\Mozilla Firefox\\";
		std::string curver = "CurrentVersion";


		//get Firefox version

		//try 64-bit Firefox
		st = RegOpenKeyExA(HKEY_LOCAL_MACHINE, subkey.c_str(), NULL, KEY_WOW64_64KEY | KEY_READ, &hKey);

		if (st != 0) {
			//try 32-bit Firefox
			st = RegOpenKeyExA(HKEY_LOCAL_MACHINE, subkey.c_str(), NULL, KEY_WOW64_32KEY | KEY_READ, &hKey);

			if (st != 0) {
				//handle error
				ErrorHandler::getInstance().onError("Couldn't open/find Firefox subkey in registry. Either error occured or Firefox is not installed.", 
													"ParserDLL error: \"Error opening registry keys\"");
				return result;
			}

			key6432 = KEY_WOW64_32KEY;	// Firefox is 32-bit
		}

		key6432 = KEY_WOW64_64KEY;	//Firefox is 64-bit

		//query FF version
		RegQueryValueExA(hKey, curver.c_str(), NULL, &dwType, (LPBYTE)buf, &len);


		//get FF install dir

		if (buf[0] != 0) {
			len = BUFSIZE;
			curver = buf;	//set detected version
			memset(buf, 0, sizeof(buf));

			subkey += curver + "\\Main\\";

			RegOpenKeyExA(HKEY_LOCAL_MACHINE, subkey.c_str(), NULL, key6432 | KEY_READ, &hKey);
			RegQueryValueExA(hKey, "Install Directory", NULL, &dwType, (LPBYTE)buf, &len);

			if (buf[0] != 0) {
				result = buf;
			}
		}

		if (buf[0] == 0) {
			//handle error
			ErrorHandler::getInstance().onError("Couldn't detect your Firefox version in registry", 
												"ParserDLL error: \"Firefox version unknown\"");
		}

		RegCloseKey(hKey);

		return result;
	}


	void FileAccessorImpl::setupPaths()
	{
		_resource_paths[size_t(EResourcePaths::INSTALLDIR)] = getFirefoxInstallDir();
		_resource_paths[size_t(EResourcePaths::PROFILES_INI)] = std::string(std::getenv("APPDATA")).append("\\Mozilla\\Firefox\\profiles.ini");
		_resource_paths[size_t(EResourcePaths::PROFILES)] = std::string(std::getenv("APPDATA")).append("\\Mozilla\\Firefox\\Profiles");
		_resource_paths[size_t(EResourcePaths::PROFILESLOCAL)] = std::string(std::getenv("LOCALAPPDATA")).append("\\Mozilla\\Firefox\\Profiles");
		_resource_paths[size_t(EResourcePaths::PROFILEPATH)] = "";
		_resource_paths[size_t(EResourcePaths::CACHE)] = "\\cache2\\entries";
		_resource_paths[size_t(EResourcePaths::DATABASE)] = "\\places.sqlite";
		_resource_paths[size_t(EResourcePaths::LOGINS)] = "\\logins.json";
	}


	void FileAccessorImpl::loadProfiles()
	{
		boost::property_tree::ptree pt;

		try	{
			boost::property_tree::ini_parser::read_ini(_resource_paths[size_t(EResourcePaths::PROFILES_INI)], pt);

			for (auto &section : pt) {
				bool isRelative = true;

				std::string name;
				std::string path;
				std::string cache_path;

				//if section is "Profile"
				if (section.first.find("Profile", 0, 7) != std::string::npos) {
					//get key values
					for (auto &key : section.second) {
						if (key.first == "Name") {
							name = key.second.get_value<std::string>();
						}
						else if (key.first == "IsRelative") {
							isRelative = key.second.get_value<bool>();
						}
						else if (key.first == "Path") {
							path = key.second.get_value<std::string>();
						}
					}

					//create paths for relative profiles
					if (isRelative) {
						path.erase(0, 9);	//remove "Profile/" 
						cache_path = _resource_paths[size_t(EResourcePaths::PROFILESLOCAL)] + "\\" + path + _resource_paths[size_t(EResourcePaths::CACHE)];
						path = _resource_paths[size_t(EResourcePaths::PROFILES)] + "\\" + path;
					}
					else {
						cache_path = path + _resource_paths[size_t(EResourcePaths::CACHE)];
					}

					//add profile
					_profile_list.push_back({ name, path, cache_path });
				}
			}
		}
		catch (const std::exception &ex) {
			//handle error
			ErrorHandler::getInstance().onError(ex.what(), "ParserDLL error: \"Error during PROFILES.INI parsing occured\"");
		}
	}


	//Interface methods

	std::string FileAccessorImpl::getPathToResource(EResourcePaths resource, size_t profile) const
	{
		if (profile < _profile_list.size()) {
			switch (resource)
			{
				case EResourcePaths::INSTALLDIR:
				case EResourcePaths::PROFILES_INI:
				case EResourcePaths::PROFILES:
				case EResourcePaths::PROFILESLOCAL:
					return _resource_paths[size_t(resource)];

				case EResourcePaths::PROFILEPATH:
					return _profile_list[profile].path;

				case EResourcePaths::CACHE:
					return _profile_list[profile].cache_path;

				case EResourcePaths::DATABASE:
				case EResourcePaths::LOGINS:
					return _profile_list[profile].path + _resource_paths[size_t(resource)];
			}
		}
		
		return "";
	}


	size_t FileAccessorImpl::getFilesCount(const std::string& dir_path) const
	{
		path p(dir_path);
		size_t count = 0;

		try	{
			if (exists(p)) {
				if (is_directory(p)) {
					for (auto& iter : directory_iterator(p)) {
						if (is_regular_file(iter)) {
							++count;
						}
					}
				}
			}
		}
		catch (const filesystem_error& ex) {
			//handle error
			ErrorHandler::getInstance().onError(ex.what(), "ParserDLL error: \"Error occured while reading cache folder contents\"");
		}

		return count;
	}


	void FileAccessorImpl::getFileList(std::vector<std::string>& list, const std::string& dir_path) const
	{
		path p(dir_path);

		try	{
			if (exists(p)) {
				if (is_directory(p)) {
					for (auto&& iter : directory_iterator(p)) {
						if (is_regular_file(iter)) {
							list.emplace_back(iter.path().filename().string());
						}
					}

					std::sort(list.begin(), list.end());
				}
			}
		}
		catch (const filesystem_error& ex) {
			//handle error
			ErrorHandler::getInstance().onError(ex.what(), "ParserDLL error: \"Error occured while reading cache folder contents\"");
		}
	}


	void FileAccessorImpl::getProfiles(std::vector<std::string>& list) const
	{
		for (auto iter : _profile_list) {
			//add profile name to vector
			list.emplace_back(iter.name);
		}
	}


	size_t FileAccessorImpl::getNumberOfProfiles() const
	{
		return _profile_list.size();
	}


	const ProfileInfo& FileAccessorImpl::getProfile(size_t profile) const
	{
		return _profile_list[profile];
	}
	
}