/*
 * TrackInfo.cpp
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
#include "TrackInfo.h"
#include "Tracer.h"

namespace FMF {

TrackInfo::TrackInfo() :
		m_db_file(),
		m_album_artist(),
		m_title(),
		m_album(),
		m_artist(),
		m_genre(),
		m_comment(),
		m_year(0),
		m_track_num(0),
		m_tracks_total(0) {
}

bool TrackInfo::validate() const {
	return !m_db_file.empty() && !m_title.empty();
}

std::ostream& operator <<(std::ostream& os, const TrackInfo& ti) {
	os << "db file: " << ti.m_db_file << std::endl;
	os << "album artist: " << ti.m_album_artist << std::endl;
	os << "title: " << ti.m_title << std::endl;
	os << "album: " << ti.m_album << std::endl;
	os << "artist: " << ti.m_artist << std::endl;
	os << "genre: " << ti.m_genre << std::endl;
	os << "comment: " << ti.m_comment << std::endl;
	os << "year: " << ti.m_year << std::endl;
	os << "track#: " << ti.m_track_num << '/' << ti.m_tracks_total << std::endl;
	return os;
}
} /* namespace fmf */
