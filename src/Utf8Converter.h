/*
 * Utf8Converter.h
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
#ifndef UTF8CONVERTER_H_
#define UTF8CONVERTER_H_

#include <string>

namespace FMF {

/**
 * converts strings from source charset to utf-8 using iconv lib.
 */
class Utf8Converter {
public:
	/**
	 * initialize with source charset for conversions to utf-8
	 *
	 * @param from_charset name of charset of source string
	 *
	 * @see EncodingDetector::detect for obtaining charset from text.
	 */
	Utf8Converter(const char* source_charset);

	Utf8Converter(const Utf8Converter&) = delete;
	Utf8Converter& operator=(const Utf8Converter&) = delete;

	~Utf8Converter();

	/**
	 * @return true if converter was successfully initialized
	 */
	bool is_open() const;

	/**
	 * convert @e str to utf-8 and place the result in @e utf8_str
	 *
	 * @return true if successful, false if failed
	 */
	bool convert(const std::string& str, std::string& utf8_str);

	/**
	 * convert @e str to utf-8 and place the result back in str if successful
	 *
	 * @return true if successful, false if failed
	 */
	bool convert(std::string& str);

private:
	std::string m_source_charset;
	void* m_handle;
};

} /* namespace FMF */
#endif /* UTF8CONVERTER_H_ */
