/*
 * MusicFilesGenerator.h
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
#ifndef FAKEMUSICFILES_H_
#define FAKEMUSICFILES_H_

#include "TrackInfo.h"

#include <string>
#include <vector>

namespace FMF {

class CDDBParser;
class Context;
class MusicFileCreator;

/**
 *
 */
class MusicFilesGenerator {
public:
	MusicFilesGenerator(Context& ctx);
	~MusicFilesGenerator();

	void operator()(MusicFileCreator& creator);

private:
	std::vector<TrackInfo> parse_cddb_file(CDDBParser& parser, const std::string& cddb_file);
	void create_fake_music_files(MusicFileCreator& creator, const std::vector<TrackInfo>& tracks, std::string& dir_path);

	Context& m_context;
};

} /* namespace FMF */
#endif /* FAKEMUSICFILES_H_ */
