/*
 * File.h
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
#ifndef MUSICFILE_H_
#define MUSICFILE_H_

#include <string>

namespace FMF {

class File {
public:
	File(const std::string& fpath);

	std::string path() const {
		return m_path;
	}

	std::string name() const {
		return m_name;
	}

	std::string extension() const {
		return m_ext;
	}

	std::string read_all() const;

	bool exists() const;

private:
	void split_path();

	std::string m_path;
	std::string m_name;
	std::string m_ext;
};

} /* namespace FMF */
#endif /* MUSICFILE_H_ */
