/*
 * MusicFileCreator.cpp
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
#include "MusicFileCreator.h"
#include "Context.h"
#include "Dir.h"
#include "Options.h"
#include "Tracer.h"
#include "TrackInfo.h"

#include <stddef.h>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/tstring.h>
#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdbool>
#include <fstream>

namespace FMF {

MusicFileCreator::MusicFileCreator(Context& ctx) :
		m_context(ctx), m_opts(ctx.options()), m_template_file(m_opts.template_music_file()), m_template_data() {
}

MusicFileCreator::~MusicFileCreator() {
}

bool MusicFileCreator::init() {
	return read_template_file();
}

bool MusicFileCreator::create_music_file(const TrackInfo& ti, std::string& dir_path) {
	if (!ti.validate()) {
		Tracer::_err("invalid track info: ", ti);
		m_context.on_create_failed();
		return false;
	}

	if (!make_dir_path(ti, dir_path)) {
		m_context.on_create_failed();
		return false;
	}

	auto out_path = m_opts.output_dir() + Dir::DIR_SEP + dir_path + Dir::DIR_SEP + make_file_name(ti);
	File fout(out_path);
	if (fout.exists()) {
		Tracer::_info("skipping existing file: ", out_path);
		m_context.on_create_skipped();
		return true;
	}

	std::ofstream os(out_path, std::ios::binary | std::ios::out);
	std::ifstream templ(m_opts.template_music_file(), std::ios::binary | std::ios::in);
	os << templ.rdbuf();
	os.close();
	templ.close();

	// taglib is not thread safe at this time Jan-2014
	std::lock_guard < std::mutex > locker(m_taglib_lock);

	TagLib::FileRef f(out_path.c_str());
	if (f.isNull()) {
		Tracer::_err("failed to create music file from template ", m_opts.template_music_file());
		m_context.on_create_failed();
		return false;
	}

	auto tag = f.tag();
	if (!tag) {
		Tracer::_err("error in taglib with template file: ", m_opts.template_music_file());
		m_context.on_create_failed();
		return false;
	}

	tag->setAlbum(TagLib::String(ti.album(), TagLib::String::UTF8));
	tag->setArtist(TagLib::String(ti.artist(), TagLib::String::UTF8));
	tag->setTitle(TagLib::String(ti.title(), TagLib::String::UTF8));
	tag->setGenre(TagLib::String(ti.genre(), TagLib::String::UTF8));
	tag->setComment(TagLib::String(ti.comment(), TagLib::String::UTF8));
	tag->setYear(ti.year());
	tag->setTrack(ti.track_num());
	tag->setComment(ti.db_file());

	bool res = f.save();
	if (res) {
		m_context.on_create_success();
		Tracer::_info("saved: ", out_path);
	}
	else {
		m_context.on_create_failed();
		Tracer::cerr("failed to save: ", res, " ", out_path);
	}
	return res;
}

bool MusicFileCreator::read_template_file() {
	std::ifstream template_file(m_opts.template_music_file(), std::ios::binary | std::ios::in);
	try {
		template_file.exceptions(std::ios_base::failbit | std::ios_base::badbit);
		auto size = template_file.seekg(0, std::ios::end).tellg();
		template_file.seekg(0, std::ios::beg);
		m_template_data.resize(size);
		template_file.read(&m_template_data[0], size);
		template_file.close();
	}
	catch (std::exception& e) {
		Tracer::_err("failed to read template file ", m_opts.template_music_file());
		return false;
	}
	return true;
}

std::string name_to_path_name(const std::string& name) {
	return (name.length() <= NAME_MAX) ? std::move(Dir::path_escape(name)) : Dir::path_escape(name.substr(0, NAME_MAX));
}

bool MusicFileCreator::make_dir_path(const TrackInfo& ti, std::string& opath) {
	std::string artist_path(name_to_path_name(ti.album_artist()));
	std::string album_path(name_to_path_name(ti.album()));
	if (artist_path.empty())
		artist_path = "Unknown";
	if (album_path.empty())
		album_path = "Unknown";
	std::string path = artist_path + Dir::DIR_SEP + album_path;
	if (path == opath) {
		return true;
	}
	Dir dir = Dir(m_opts.output_dir()).sub_dir(artist_path);
	if (!dir.exists() && !dir.create()) {
		Tracer::_err("failed to create diretory ", dir.path());
		return false;
	}
	dir = dir.sub_dir(album_path);
	if (!dir.exists() && !dir.create()) {
		Tracer::_err("failed to create diretory ", dir.path());
		return false;
	}
	opath = path;
	return true;
}

std::string MusicFileCreator::make_file_name(const TrackInfo& ti) {
	constexpr const char INDEX_DELIM[] = " - ";
	static_assert( sizeof(INDEX_DELIM) == sizeof(" - "), "");
	const int index_width = std::max(2U, static_cast<unsigned>(::log10(ti.tracks_total())));
	const int reserved_len = m_template_file.extension().length() + 1 + index_width + sizeof(INDEX_DELIM) - 1;
	const size_t max_name_len = NAME_MAX - reserved_len;
	std::string fname = ti.title();
	if (fname.length() > max_name_len) {
		fname = fname.substr(0, std::min(max_name_len, ti.title().length()));
	}
	fname = Dir::path_escape(fname);
	std::ostringstream os;

	os << std::setw(index_width) << std::setfill('0') << ti.track_num() << INDEX_DELIM << fname << '.'
			<< m_template_file.extension();
	return os.str();
}

} /* namespace FMF */
