/*
 * EncodingDetector.h
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
#ifndef ENCODINGDETECTOR_H_
#define ENCODINGDETECTOR_H_

#include <uchardet/uchardet.h>
#include <string>

namespace FMF {

/**
 * detects charset of text using lib uchardet
 *
 * http://lxr.mozilla.org/seamonkey/source/extensions/universalchardet/
 */
class EncodingDetector {
public:
	EncodingDetector() :
			m_handle(uchardet_new()) {
	}

	~EncodingDetector() {
		uchardet_delete(m_handle);
	}

	const char* detect(const std::string& text) {
		uchardet_reset(m_handle);
		uchardet_handle_data(m_handle, text.data(), text.size());
		uchardet_data_end(m_handle);
		return uchardet_get_charset(m_handle);
	}

private:
	uchardet_t m_handle;
};

} /* namespace FMF */
#endif /* ENCODINGDETECTOR_H_ */
