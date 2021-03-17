/*
 * CovScript Manager: OS Depended Functions Implementation
 * Licensed under Apache 2.0
 * Copyright (C) 2020-2021 Chengdu Covariant Technologies Co., LTD.
 * Website: https://covariant.cn/
 * Github:  https://github.com/chengdu-zhirui/
 */
#include <csman/global.hpp>
#include <iostream>
#include <cstring>
#include <cstdio>

#include <covscript_impl/dirent/dirent.hpp>

namespace csman_impl {
	enum class path_type {
		unknown = DT_UNKNOWN, fifo = DT_FIFO, sock = DT_SOCK, chr = DT_CHR, dir = DT_DIR, blk = DT_BLK, reg = DT_REG, link = DT_LNK
	};
	/**
	 * scan the entries of path
	 * @tparam container_t: type of container
	 * @tparam exception_t:    type of exception
	 * @tparam function_t:     type of function, must be bool(path_type, const std::string&)
	 */
	template<typename container_t, typename exception_t, typename function_t>
	container_t scan(const std::string& path, function_t &&func)
	{
		DIR *dir = ::opendir(path.c_str());
		if (dir == nullptr)
			throw exception_t("Path does not exist.");
		container_t entries;
		for (dirent *dp = ::readdir(dir); dp != nullptr; dp = ::readdir(dir)) {
			if (func(static_cast<path_type>(dp->d_type)))
				entries.emplace_back(dp->d_name);
		}
		::closedir(dir);
		return std::move(entries);
	}
}

namespace csman::sys::file {
	bool create(std::string path)
	{
		path = csman::str::to_file_path(path);
		std::ofstream ofs;
		for(int i = path.size()-1; i>=0 ; i--)
			if(path[i] == delimiter) {
				sys::dir::create(path.substr(0,i));
				ofs.open(path);
				break;
			}
		ofs.close();
		ofs.open(path);
		if(!ofs.is_open()) {
			ofs.close();
			return false;
		}
		ofs.close();
		return true;
	}
	std::vector<std::string> scan(std::string path)
	{
		path = csman::str::to_file_path(path);
		return csman_impl::scan<std::vector<std::string>, std::runtime_error>(path, [](csman_impl::path_type t) {
			return t != csman_impl::path_type::dir;
		});
	}
}

namespace csman::sys::dir {
	std::vector<std::string> scan(std::string path)
	{
		path = csman::str::to_dir_path(path);
		return csman_impl::scan<std::vector<std::string>, std::runtime_error>(path, [](csman_impl::path_type t) {
			return t == csman_impl::path_type::dir;
		});
	}
}
