/*
 * TrackInfo.h
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
#ifndef TRACKINFO_H_
#define TRACKINFO_H_

#include <string>
#include <iosfwd>

namespace FMF {

/**
 * parsed track info.
 *
 * all values set must already be in utf-8 encoding.
 */
class TrackInfo {
public:
	TrackInfo();

	/**
	 * @return true if valid. a valid track info must have at least a non empty title and db file.
	 */
	bool validate() const;

	friend std::ostream& operator<<(std::ostream& os, const TrackInfo& ti);

	void set_album(const std::string album) {
		m_album = album;
	}

	void set_artist(const std::string artist) {
		m_artist = artist;
	}

	void set_comment(const std::string comment) {
		m_comment = comment;
	}

	void set_db_file(const std::string dbFile) {
		m_db_file = dbFile;
	}

	void set_album_artist(const std::string album_artist) {
		m_album_artist = album_artist;
	}

	void set_genre(const std::string genre) {
		m_genre = genre;
	}

	void set_title(const std::string title) {
		m_title = title;
	}

	void set_track_num(size_t trackNum) {
		m_track_num = trackNum;
	}

	void set_tracks_total(size_t tracksTotal) {
		m_tracks_total = tracksTotal;
	}

	void set_year(size_t year) {
		m_year = year;
	}

	std::string album() const {
		return m_album;
	}

	std::string artist() const {
		return m_artist;
	}

	std::string comment() const {
		return m_comment;
	}

	std::string db_file() const {
		return m_db_file;
	}

	std::string album_artist() const {
		return m_album_artist;
	}

	std::string genre() const {
		return m_genre;
	}

	std::string title() const {
		return m_title;
	}

	size_t track_num() const {
		return m_track_num;
	}

	size_t tracks_total() const {
		return m_tracks_total;
	}

	size_t year() const {
		return m_year;
	}

private:
	std::string m_db_file;
	std::string m_album_artist;
	std::string m_title;
	std::string m_album;
	std::string m_artist;
	std::string m_genre;
	std::string m_comment;
	size_t m_year;
	size_t m_track_num;
	size_t m_tracks_total;
};

} /* namespace fmf */
#endif /* TRACKINFO_H_ */
