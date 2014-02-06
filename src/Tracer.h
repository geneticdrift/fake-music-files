/*
 * Trace.h
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
#ifndef TRACE_H_
#define TRACE_H_

#include <mutex>
#include <iosfwd>
#include <iostream>
#include <sstream>

namespace FMF {

/**
 * trace
 */
class Tracer {
public:
	enum TraceLevel {
		None = 0,
		Debug = 1,
		Info = 2,
		Warn = 4,
		Err = 8,
		All = (Debug | Info | Warn | Err),
		V0 = Err,
		V1 = (Warn | Err),
		V2 = (Info | Warn | Err),
		V3 = (All)
	};

private:
	static TraceLevel sTraceLevel;

	static std::mutex sTraceMutext;

	static inline void TraceLine(std::ostream& os) {
		os << std::endl;
	}

	template<typename T, typename ... Args>
	static void TraceLine(std::ostream& os, const T& arg, const Args& ... args) {
		os << std::boolalpha << arg;
		TraceLine(os, args...);
	}

	template<typename ... T>
	static void Trace(TraceLevel level, const T& ... args) {
		if (!level || ((sTraceLevel | Err) & level)) {
			std::ostringstream os;
			TraceLine(os, args...);
			std::unique_lock < std::mutex > lock(sTraceMutext);
			((level & Err) ? std::cerr : std::cout) << os.str();
		}
	}

public:
	inline static void set_verbosity(size_t v_level) {
		TraceLevel t_level = TraceLevel::None;
		switch (v_level) {
		case 0:
			t_level = TraceLevel::V0;
			break;
		case 1:
			t_level = TraceLevel::V1;
			break;
		case 2:
			t_level = TraceLevel::V2;
			break;
		case 3:
		default:
			t_level = TraceLevel::V3;
			break;
		}
		set_trace_level(t_level);
	}

	inline static void set_trace_level(TraceLevel level) {
		sTraceLevel = level;
		_debug("trace level: ", level);
	}

	/**
	 * unconditional output to std::cout
	 */
	template<typename ... T>
	static void cout(const T& ... args) {
		Trace(None, args...);
	}

	/**
	 * unconditional output to std::cerr
	 */
	template<typename ... T>
	static void cerr(const T& ... args) {
		Trace(Err, args...);
	}

	template<typename ... T>
	static void _debug(const T& ... args) {
		Trace(Debug, "[DEBUG] ", args...);
	}

	template<typename ... T>
	static void _info(const T& ... args) {
		Trace(Info, "[INFO] ", args...);
	}

	template<typename ... T>
	static void _warn(const T& ... args) {
		Trace(Warn, "[WARNING] ", args...);
	}

	template<typename ... T>
	static void _err(const T& ... args) {
		Trace(Err, "[ERROR] ", args...);
	}
};

} /* namespace FMF */
#endif /* TRACE_H_ */
