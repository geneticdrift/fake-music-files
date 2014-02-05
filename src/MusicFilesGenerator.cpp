/*
 * MusicFilesGenerator.cpp
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
#include "MusicFilesGenerator.h"
#include "CDDBParser.h"
#include "Options.h"
#include "TrackInfo.h"
#include "Tracer.h"
#include "MusicFileCreator.h"

#include <sstream>
#include <thread>

namespace FMF {

class ParseFailureException: public std::exception {
public:
	ParseFailureException(const std::string& cddb_file) :
			m_msg("cddb parse failed ") {
		m_msg += cddb_file;
	}
	virtual const char* what() const noexcept {
		return m_msg.c_str();
	}
private:
	std::string m_msg;
};

MusicFilesGenerator::MusicFilesGenerator(Context& ctx) :
		m_context(ctx) {
}

MusicFilesGenerator::~MusicFilesGenerator() {
}

void MusicFilesGenerator::operator ()(MusicFileCreator& creator) {
	Tracer::_debug("generator enter thread ", std::this_thread::get_id());
	CDDBParser parser(m_context);
	std::string db_file_path;
	std::string last_dir_path;
	while (!Context::stopped() && m_context.pick_db_file(db_file_path)) {
		try {
			create_fake_music_files(creator, parse_cddb_file(parser, db_file_path), last_dir_path);
		}
		catch (ParseFailureException& e) {
			continue;
		}
		catch (std::exception& e) {
			Tracer::_err("exception: ", e.what());
			break;
		}
	}
	Tracer::_debug("generator exit thread ", std::this_thread::get_id());
}

std::vector<TrackInfo> MusicFilesGenerator::parse_cddb_file(CDDBParser& parser, const std::string& cddb_file) {
	if (!File(cddb_file).exists()) {
		m_context.on_parse_failed(cddb_file);
		Tracer::_err("file not found ", cddb_file, " (if cddb directory changed use --update to recreate the cache)");
		throw ParseFailureException(cddb_file);
	}
	if (!parser.parse(cddb_file)) {
		m_context.on_parse_failed(cddb_file);
		Tracer::_err("failed Parser: ", parser);
		throw ParseFailureException(cddb_file);
	}
	m_context.on_parse_success();
	return parser.getTracks();
}

void MusicFilesGenerator::create_fake_music_files(MusicFileCreator& creator, const std::vector<TrackInfo>& tracks,
		std::string& dir_path) {
	for (auto& ti : tracks) {
		if (Context::stopped())
			break;
		creator.create_music_file(ti, dir_path);
	}
}

} /* namespace FMF */
