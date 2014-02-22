/*
 * Utf8Converter.cpp
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
#include "Utf8Converter.h"
#include "Tracer.h"

#include <errno.h>
#include <iconv.h>
#include <string.h>
#include <algorithm>

namespace FMF {

Utf8Converter::Utf8Converter(const char* source_charset) :
		m_source_charset(source_charset), m_handle((::iconv_t) -1) {
	m_handle = ::iconv_open("UTF-8", m_source_charset.c_str());
	if ((::iconv_t) -1 == m_handle) {
		int err = errno;
		Tracer::_err("iconv: failed to open: errno=", err, " '", ::strerror(err), "' with charset '", m_source_charset,
				'\'');
	}
}

Utf8Converter::~Utf8Converter() {
	::iconv_close(m_handle);
}

bool Utf8Converter::is_open() const {
	return (::iconv_t) -1 != m_handle;
}

bool Utf8Converter::convert(const std::string& str, std::string& utf8_str) {
	if (str.empty()) {
		utf8_str = str;
		return true;
	}
	if (m_source_charset.empty()) {
		// this can be either 7bit ascii or undetected charset
		if (str.end() == std::find_if(str.begin(), str.end(), [](std::string::value_type c) {return (unsigned)c > 0x7f;})) {
			// ok, 7bit ascii
			utf8_str = str;
			return true;
		}
		Tracer::_err("charset of '", str, "' is unknown but it contains non ascii chars > 0x7f");
		return false;
	}
	const size_t utf8_max_len = str.size() * 4;
	utf8_str.resize(utf8_max_len);
	char* in = const_cast<char*>(&str[0]);
	size_t in_remain = str.size();
	char* out = &utf8_str[0];
	size_t out_remain = utf8_max_len;
	int err = ::iconv(m_handle, &in, &in_remain, &out, &out_remain);
	if (err) {
		Tracer::_err("failed to convert '", str, "' from charset ", m_source_charset, " to utf-8");
		return false;
	}
	utf8_str.resize(utf8_max_len - out_remain);
	return true;
}

bool Utf8Converter::convert(std::string& str) {
	std::string utf8;
	if (!convert(str, utf8))
		return false;
	str = std::move(utf8);
	return true;
}

}
/* namespace FMF */
