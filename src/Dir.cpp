/*
 * Dir.cpp
 *
 * This file is part of fmf
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Copyright 2014 GD <gd@iotide.com>
 */
#include "Dir.h"
#include <string.h>
#include <error.h>
#include <errno.h>
#include <sys/stat.h>
#include <algorithm>

namespace FMF {

Dir::Dir(const std::string& name) :
		m_path(name) {
}

bool Dir::for_each(DirScanHandler& handler) {
	DIR* d = opendir(m_path.c_str());
	if (!d)
		::error(errno, errno, "opendir %s", m_path.c_str());
	handler.on_dir_begin(*this);
	struct dirent de, *dep;
	int err = 0;
	EachResult res = EachResult::CONTINUE;
	while (res != EachResult::STOP && !(err = readdir_r(d, &de, &dep)) && dep) {
		if (dep->d_name[0] == '.' && (dep->d_name[1] == '\0' || (dep->d_name[1] == '.' && dep->d_name[2] == '\0')))
			continue;
		res = handler.on_dir_entry(*this, *dep);
	}
	closedir(d);
	handler.on_dir_end(*this);
	if (err)
		::error(err, err, "readdir %s", m_path.c_str());
	return res == EachResult::CONTINUE;
}

time_t Dir::mod_time() const {
	struct stat st;
	int err = ::stat(m_path.c_str(), &st);
	if (err)
		::error(err, err, "stat %s", m_path.c_str());
	return st.st_mtime;
}

Dir::Dir(const Dir& dir) :
		m_path(dir.m_path) {
}

Dir& Dir::operator =(const Dir& dir) {
	if (this != &dir) {
		m_path = dir.m_path;
	}
	return *this;
}

Dir::Dir(const Dir&& dir) :
		m_path(std::move(dir.m_path)) {
}

Dir& Dir::operator=(const Dir&& dir) {
	if (this != &dir) {
		m_path = std::move(dir.m_path);
	}
	return *this;
}

bool Dir::exists() const {
	struct stat st;
	int err = ::stat(m_path.c_str(), &st);
	if (errno == ENOENT)
		return false;
	if (err)
		::error(err, err, "stat %s", m_path.c_str());

	return st.st_mode == S_IFDIR;
}

bool Dir::create() {
	if (::mkdir(m_path.c_str(), 0755)) {
		int err = errno;
		if (err != EEXIST)
			::error(err, err, "mkdir %s", m_path.c_str());
	}
	return true;
}

std::string Dir::path_escape(const std::string& path_part) {
	std::string str;
	str.resize(path_part.size());
	std::transform(path_part.begin(), path_part.end(), str.begin(),
			[](std::string::value_type c) {return ::strchr("/:\\", c) ? '_' : c;});
	return str;
}

}/* namespace FMF */
