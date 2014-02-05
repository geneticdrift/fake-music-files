/*
 * Dir.h
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
#ifndef DIR_H_
#define DIR_H_

#include <dirent.h>
#include <functional>
#include <string>

namespace FMF {

class Dir {
public:
	Dir(const std::string& path);
	Dir(const Dir& dir);
	Dir& operator=(const Dir& dir);
	Dir(const Dir&& dir);
	Dir& operator=(const Dir&& dir);

	static const char DIR_SEP = '/';

	enum class EachResult {
		SKIP, STOP, CONTINUE
	};

	struct DirScanHandler: public std::binary_function<const Dir&,
			const struct dirent&, EachResult> {
		virtual void on_dir_begin(const Dir& dir) = 0;

		/// @return EachResult::STOP to stop the iteration or EachResult::CONTINUE to keep scannning.
		virtual EachResult on_dir_entry(const Dir& dir,
				const struct dirent& de) = 0;

		virtual void on_dir_end(const Dir& dir) = 0;
	};

	/**
	 * iterate dir entries and call handler for each of them except . and ..
	 *
	 * @param handler - {@link DirHandler}
	 * @return false if iteration was stopped by handler.
	 */
	bool for_each(DirScanHandler& handler);

	bool exists() const;

	bool create();

	const std::string& path() const {
		return m_path;
	}

	Dir sub_dir(const std::string& name) const {
		return Dir(path() + Dir::DIR_SEP + name);
	}

	time_t mod_time() const;

	static std::string path_escape(const std::string& path_part);

private:
	std::string m_path;
};

}/* namespace FMF */
#endif /* DIR_H_ */
