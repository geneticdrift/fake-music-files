/*
 * CDDBParser.cpp
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
#include "CDDBParser.h"
#include "TrackInfo.h"
#include "Tracer.h"
#include "EncodingDetector.h"
#include "File.h"
#include "Utf8Converter.h"

#include <iosfwd>
#include <fstream>
#include <sstream>
#include <istream>

namespace FMF {

/**
 * abstract base line parser.
 */
class KeywordParser {
public:
	KeywordParser(const char* keyword) :
			m_keyword(keyword), m_started(false), m_done(false) {
	}

	virtual ~KeywordParser() {
	}

	virtual bool match(const std::string& line) = 0;

	void parse(const std::string& line) {
		assert(!m_done);
		bool kw_found = match(line);
		if (kw_found) {
			m_started = true;
		} else if (m_started) {
			m_done = true;
		}
	}

	bool is_done() const {
		return m_done;
	}

	virtual void finish() = 0;

	virtual bool validate(Utf8Converter& conv) = 0;

	virtual void reset() {
		m_started = false;
		m_done = false;
	}

	virtual std::ostream& write(std::ostream& os) const {
		os << std::boolalpha << "keyword: " << m_keyword << ", started: " << m_started << ", done: " << m_done;
		return os;
	}

protected:
	std::string m_keyword;bool m_started;bool m_done;
};

/**
 * keyword=value line parser
 */
class PlainKeywordParser: public KeywordParser {
	typedef KeywordParser inherited;
public:
	PlainKeywordParser(const char* keyword) :
			KeywordParser(keyword) {
	}

	virtual bool match(const std::string& line) {
		bool matched = !strncmp(line.c_str(), m_keyword.c_str(), m_keyword.length());
		if (matched) {
			size_t pos = m_keyword.length();
			m_value.append(line.substr(pos, std::string::npos));
		}
		return matched;
	}

	virtual void finish() {
		// noop
	}

	virtual bool validate(Utf8Converter& conv) {
		return conv.convert(m_value);
	}

	virtual void reset() {
		inherited::reset();
		m_value.clear();
	}

	const std::string& value() const {
		return m_value;
	}

	virtual std::ostream& write(std::ostream& os) const {
		return KeywordParser::write(os) << ", value: " << m_value;
	}

private:
	std::string m_value;
};

/**
 * keywordN=value line parser
 */
class NumberedKeywordParser: public KeywordParser {
	typedef KeywordParser inherited;
public:
	NumberedKeywordParser(const char* keyword) :
			KeywordParser(keyword), m_num(std::string::npos) {
	}

	virtual bool match(const std::string& line) {
		unsigned num;
		bool matched = 1 == sscanf(line.c_str(), m_keyword.c_str(), &num);
		if (matched) {
			if (m_num == std::string::npos) {
				m_num = num;
			} else if (m_num != num) {
				finish();
				m_num = num;
			}
			size_t num_size = (num ? log10(num) : 0);
			size_t pos = m_keyword.length() - 1 + num_size;
			m_value.append(line.substr(pos, std::string::npos));
		}
		return matched;
	}

	int num() const {
		return m_num;
	}

	virtual void finish() {
		m_values.push_back(m_value);
		m_value.clear();
	}

	virtual bool validate(Utf8Converter& conv) {
		for (auto& val : m_values) {
			if (!conv.convert(val))
				return false;
		}
		return true;
	}

	virtual void reset() {
		inherited::reset();
		m_num = std::string::npos;
		m_values.clear();
	}

	const std::vector<std::string>& values() const {
		return m_values;
	}

	virtual std::ostream& write(std::ostream& os) const {
		KeywordParser::write(os) << ", values: [";
		for (auto& val : m_values) {
			os << val << ", ";
		}
		return os << "], num: " << m_num;
	}

private:
	unsigned m_num;
	std::string m_value;
	std::vector<std::string> m_values;
};

CDDBParser::CDDBParser(Context& ctx) :
		m_context(ctx), m_valid(false), m_dtitle_parser(new PlainKeywordParser("DTITLE=")), m_dyear_parser(
				new PlainKeywordParser("DYEAR=")), m_dgenre_parser(new PlainKeywordParser("DGENRE=")), m_ttitle_parser(
				new NumberedKeywordParser("TTITLE%d=")), m_parsers(), m_charset() {
	m_parsers.push_back(m_dtitle_parser);
	m_parsers.push_back(m_dyear_parser);
	m_parsers.push_back(m_dgenre_parser);
	m_parsers.push_back(m_ttitle_parser);
}

CDDBParser::~CDDBParser() {
	for (auto parser : m_parsers) {
		delete parser;
	}
	m_parsers.clear();
}

bool CDDBParser::parse(const std::string& path) {
	reset();

	m_db_file = path;
	Tracer::_info("parsing: ", path);

	// get the file contents into a string stream
	File db_file(path);
	std::istringstream content(db_file.read_all());

	// detect file's charset
	EncodingDetector detector;
	m_charset = detector.detect(content.str());
	Utf8Converter conv(m_charset.c_str());
	if (!conv.is_open()) {
		// conversion not available
		return false;
	}

	auto parser = m_parsers.begin();
	auto parse_end = m_parsers.end();

	std::string line;
	std::getline(content, line);

	while (content && parser != parse_end) {
		// skip empty line (although are illegal in cddb) and comment lines
		if (line.empty() || line.at(0) == '#') {
			std::getline(content, line);
			continue;
		}
		(*parser)->parse(line);
		if ((*parser)->is_done()) {
			(*parser)->finish();
			parser++;
		} else {
			std::getline(content, line);
		}
	}

	m_valid = true;

	for (auto parser : m_parsers) {
		if (!parser->is_done() || !parser->validate(conv)) {
			m_valid = false;
			break;
		}
	}

	if (m_valid && m_ttitle_parser->values().empty()) {
		Tracer::_warn(path, " parsed but contains no tracks");
		m_valid = false;
	}

	return m_valid;
}

void CDDBParser::reset() {
	m_valid = false;
	m_db_file.clear();
	for (auto parser : m_parsers) {
		parser->reset();
	}
}

std::vector<TrackInfo> CDDBParser::getTracks() const {
	std::vector<TrackInfo> tracks;
	if (!m_valid) {
		return std::move(tracks);
	}

	auto disk_artist_title = splitValue(m_dtitle_parser->value());
	std::string disk_artist = disk_artist_title.first;
	std::string disk_title = disk_artist_title.second;
	std::string disk_genre = m_dgenre_parser->value();
	std::string disk_year = m_dyear_parser->value();

	size_t track_num = 1;
	const bool skip_empty_titles = m_context.options().skip_empty_titles();
	auto titles = m_ttitle_parser->values();
	for (auto& track_title_value : titles) {
		auto track_artist_title = splitValue(track_title_value);
		if (skip_empty_titles && track_artist_title.second.empty()) {
			continue;
		}
		TrackInfo ti;
		ti.set_db_file(m_db_file);
		ti.set_title(track_artist_title.second);
		ti.set_album(disk_title);
		ti.set_artist(track_artist_title.first.empty() ? disk_artist : track_artist_title.first);
		std::istringstream is(disk_year);
		size_t year = 0;
		is >> year;
		ti.set_year(year);
		ti.set_genre(disk_genre);
		ti.set_track_num(track_num++);
		ti.set_tracks_total(titles.size());
		tracks.push_back(ti);
	}
	return tracks;
}

std::ostream& operator <<(std::ostream& os, const CDDBParser& db_parser) {
	for (auto parser : db_parser.m_parsers) {
		os << "{ ";
		parser->write(os);
		os << " }" << std::endl;
	}
	return os;
}

/*static*/std::pair<std::string, std::string> CDDBParser::splitValue(const std::string& value) {
	size_t pos = value.find(VALUE_SEPARATOR, 0);
	if (pos == std::string::npos) {
		return std::make_pair(std::string(), value);
	}
	return std::make_pair(value.substr(0, pos), value.substr(pos + sizeof(VALUE_SEPARATOR) - 1, std::string::npos));
}

} /* namespace fmf */
