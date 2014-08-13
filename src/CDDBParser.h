/*
 * CDDBParser.h
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
#ifndef PARSERS_H_
#define PARSERS_H_

#include "TrackInfo.h"
#include "Context.h"

#include <iostream>
#include <string.h>
#include <vector>
#include <cassert>
#include <cmath>
#include <utility>

namespace FMF {

class KeywordParser;
class PlainKeywordParser;
class NumberedKeywordParser;

/**
 * parser of freecddb file.
 *
 * http://ftp.freedb.org/pub/freedb/misc/freedb_database_format_specs.zip
 *
 * All of the applicable keywords must be present in the file, though they may
 * have empty data except for the DISCID and DTITLE keywords. They must appear in
 * the file in the order shown below. Multiple occurrences of the same keyword
 * indicate that the data contained on those lines should be concatenated; this
 * applies to any of the textual fields.
 * Keywords with numeric data should not have a comma after the last number on
 * each line.
 */
class CDDBParser {
public:
	static constexpr const char VALUE_SEPARATOR[] = " / ";

	CDDBParser(Context& ctx);
	~CDDBParser();

	/**
	 * parse the file at {@e path}
	 *
	 * @return true for success, or false for failure
	 */
	bool parse(const std::string& path);

	/**
	 * get tracks info after successful parse
	 */
	std::vector<TrackInfo> getTracks() const;

	friend std::ostream& operator<<(std::ostream& os, const CDDBParser& db_parser);

private:
	/**
	 * prepare for another parse
	 */
	void reset();

	/**
	 * splits {@e value} on a " / " token.
	 *
	 * in freecddb format this is: "artist / title"
	 */
	static std::pair<std::string, std::string> splitValue(const std::string& value);

	Context& m_context;

	bool m_valid;

	std::string m_db_file;

	PlainKeywordParser* m_dtitle_parser;
	PlainKeywordParser* m_dyear_parser;
	PlainKeywordParser* m_dgenre_parser;
	NumberedKeywordParser* m_ttitle_parser;

	std::vector<KeywordParser*> m_parsers;

	std::string m_charset;
};

} /* namespace fmf */
#endif /* PARSERS_H_ */
