/*
 * File.cpp
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
#include "File.h"
#include "Dir.h"

#include <errno.h>
#include <error.h>
#include <sys/stat.h>
#include <fstream>
#include <sstream>

namespace FMF {

File::File(const std::string& fpath) {
	m_path = fpath;
	split_path();
}

std::string File::read_all() const {
	std::ifstream is(m_path, std::ios::binary | std::ios::in);
	std::ostringstream content(std::ios::binary | std::ios::out);
	content << is.rdbuf();
	is.close();
	return std::move(content.str());
}

bool File::exists() const {
	struct stat st;
	int err = ::stat(m_path.c_str(), &st);
	if (err) {
		if (errno == ENOENT)
			return false;
		::error(err, err, "stat %s", m_path.c_str());
	}
	return true;
}

void File::split_path() {
	auto npos = m_path.rfind(Dir::DIR_SEP, std::string::npos);
	auto epos = m_path.rfind('.', std::string::npos);
	if (npos == std::string::npos)
		npos = 0;
	else
		npos++;

	if (epos != std::string::npos && epos < npos)
		epos = m_path.length();

	m_name = m_path.substr(npos, epos - npos);
	epos = std::min(epos + 1, m_path.length());
	m_ext = m_path.substr(epos, m_path.length() - epos);
}

}/* namespace FMF */
