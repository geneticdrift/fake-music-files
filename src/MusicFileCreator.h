/*
 * MusicFileCreator.h
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
#ifndef MUSICFILECREATOR_H_
#define MUSICFILECREATOR_H_

#include "File.h"

#include <mutex>
#include <string>
#include <vector>

namespace FMF {

class Context;
class Options;
class TrackInfo;

/**
 * creates fake music files from template Options::template_music_file()}
 */
class MusicFileCreator {
public:
	MusicFileCreator(Context& ctx);
	~MusicFileCreator();

	bool init();

	/**
	 * create fake music file for track info @e ti
	 *
	 * @param ti 		track info for tagging the created file
	 * @param dir_path	saves the last dir path
	 *
	 * @return true - if successful, false if failed
	 */
	bool create_music_file(const TrackInfo& ti, std::string& dir_path);

private:
	bool read_template_file();

	bool make_dir_path(const TrackInfo& ti, std::string& opath);

	std::string make_file_name(const TrackInfo& ti);

	Context& m_context;
	const Options& m_opts;
	std::mutex m_taglib_lock;
	const File m_template_file;
	std::vector<char> m_template_data;
};

} /* namespace FMF */
#endif /* MUSICFILECREATOR_H_ */
