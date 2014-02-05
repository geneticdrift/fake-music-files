/*
 * Options.cpp
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
#include "Options.h"
#include "config.h"
#include "Tracer.h"

#include <errno.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>
#include <taglib/fileref.h>
#include <unistd.h>
#include <cstdio>
#include <iostream>

namespace FMF {

/**
 * max number of cds to accept in command line option -n, --num-albums
 */
const int Options::MAX_CDS = 10e6;

/**
 * max number of threads to accept in command line option -c, --threads
 */
const int Options::MAX_THREADS = 10e6;

/**
 * default value for command line option -t, --template
 */
#ifndef FMF_TEMPLATES_PATH
#define FMF_TEMPLATES_PATH "./template"
#endif
const char* Options::DEFAULT_TEMPLATE = FMF_TEMPLATES_PATH "/template.mp3";

static int s_long_opt;

struct option Options::s_options[] = {
										{
											"cddb",
											required_argument,
											0,
											'd' },
										{
											"out",
											required_argument,
											0,
											'o' },
										{
											"num-albums",
											required_argument,
											0,
											'n' },
										{
											"in",
											required_argument,
											0,
											'i' },
										{
											"template",
											required_argument,
											0,
											't' },
										{
											"verbose",
											no_argument,
											0,
											'v' },
										{
											"update",
											no_argument,
											0,
											'u' },
										{
											"threads",
											required_argument,
											0,
											'c' },
										{
											"help",
											no_argument,
											0,
											'h' },
										{
											"version",
											no_argument,
											&s_long_opt,
											'v' },
										{
											"usage",
											no_argument,
											&s_long_opt,
											'u' },
										{
											0,
											0,
											0,
											0 } };

const char* s_version =
	R"(%PACKAGE_NAME%
Copyright (C) %LICENSE_YEAR% iotide.com
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
)";

const char* s_usage =
		R"(Usage:
    %PROGRAM% --cddb <cddb directory> --out <output directory>

Options:
    -d, --cddb       The CDDB directory.
                     This is the directory where CDDB archive was extracted.
                     CDDB files will be picked randomly from this database.
    
    -i, --in         Path to a CDDB file.
                     This can be used instead of -d in order to use a specific CDDB file as input.
    
    -o, --out        Output directory.
                     This is the directory where fake music files will be generated.
                     Sub directories will be created for the titles: ARTIST/ALBUM/TITLE
    
    -n, --num-albums Number of CDDB files to use for generating fake music files.
                     Default: 1
    
    -u, --update     Update the CDDB cache.
                     fmf creates a cache with lists of all the CDDB files by scanning the CDDB directory (-d).
                     Use this option to force a re-scan of the CDDB directory.
                     The cache will be automatically updated if the value of -d option does not match the previously cached directory.
    
    -t, --template   The template music file to use for creating fake music files.
                     This file will be copied and renamed and then tagged for each generated fake music file.
                     Keep it small if generating many files.
                     Default: %DEFAULT_TEMPLATE%
    
    -c, --threads    The number of threads to use.
                     Default: 1
    
    -v, --verbose    Increase output verbosity.
    
        --version    Output version.
    
    -h, --help
    	--usage      Output this message.
)";

std::string replace(const char* str, const char* find, const char* replace) {
	std::string s(str);
	std::string f(find);
	auto pos = s.find(f);
	if (pos == std::string::npos)
		return s;
	return s.replace(pos, f.length(), std::string(replace));
}

void version(std::ostream& os) {
	std::string ver = s_version;
	ver = replace(ver.c_str(), "%PACKAGE_NAME%", PACKAGE_STRING);
	ver = replace(ver.c_str(), "%LICENSE_YEAR%", LICENSE_YEAR);
	os << ver;
}

void Options::usage(const char* prog_name, std::ostream& os) {
	os << PACKAGE_STRING << std::endl;
	std::string use = s_usage;
	use = replace(use.c_str(), "%PROGRAM%", prog_name);
	use = replace(use.c_str(), "%DEFAULT_TEMPLATE%", Options::DEFAULT_TEMPLATE);
	os << use;
}

std::string int2str(int i) {
	char str[static_cast<int>(std::log10(std::numeric_limits<int>::max())) + 2];
	::snprintf(str, sizeof(str), "%d", i);
	return std::string(str);
}

int str2int(const char* arg) {
	int val = -1, n;
	if (1 == ::sscanf(arg, "%d", &n) && int2str(n) == arg) {
		val = n;
	}
	return val;
}

Options::Options() :
		m_db_dir(), m_output_dir(), m_num_albums(0), m_db_file(), m_template_music_file(DEFAULT_TEMPLATE), m_skip_empty_titles(
				true), m_verbosity(1), m_update_cache(false), m_num_threads(0), m_valid(true), m_output_dir_set(false), m_db_dir_set(
				false), m_num_albums_set(false), m_db_file_set(false), m_template_music_file_set(false), m_num_threads_set(
				false) {
}

Options::~Options() {
}

bool Options::parse(int argc, char** argv) {
	int option_index = 0;
	::opterr = 0;
	while (true) {
		char c = getopt_long(argc, argv, ":d:o:n:i:t:vuc:h", s_options, &option_index);

		if (c == -1)
			break;
//		Tracer::_debug("option: ", (int) c, " = ", c, " option_index: ", option_index, " optind: ", ::optind,
//				" optopt: ", optopt, " = ", (char) ::optopt, " s_options[option_index].name: ",
//				s_options[option_index].name);
		switch (c) {
		case 0:
			// long option
			switch (s_long_opt) {
			case 'v':
				version(std::cout);
				::exit(EXIT_SUCCESS);
			case 'u':
				usage(argv[0], std::cout);
				::exit(EXIT_SUCCESS);
			}
			break;
		case 'd':
			if (m_db_file_set) {
				Tracer::cerr("can't set both db file (-i, --in) and db dir (-d, --cddb)");
				m_valid = false;
			}
			set_real_path(m_db_dir, optarg, "-d, --cddb", "db dir");
			m_db_dir_set = true;
			break;
		case 'o':
			set_real_path(m_output_dir, optarg, "-o, --out", "output dir");
			m_output_dir_set = true;
			break;
		case 'n':
			m_num_albums = str2int(optarg);
			m_num_albums_set = true;
			break;
		case 'i':
			if (m_db_dir_set) {
				Tracer::cerr("can't set both db file (-i, --in) and db dir (-d, --cddb)");
				m_valid = false;
			}
			set_real_path(m_db_file, optarg, "-i, --in", "db file");
			m_db_file_set = true;
			break;
		case 't':
			set_real_path(m_template_music_file, optarg, "-t, --template", "template music file");
			m_template_music_file_set = true;
			break;
		case 'v':
			m_verbosity++;
			break;
		case 'u':
			m_update_cache = true;
			break;
		case 'c':
			m_num_threads = str2int(optarg);
			m_num_threads_set = true;
			break;
		case 'h':
			usage(argv[0], std::cout);
			::exit(EXIT_SUCCESS);
		case '?':
			if (::optopt) {
				Tracer::cerr("unknown option: ", (char) ::optopt);
			}
			else {
				Tracer::cerr("unknown option: ", argv[::optind - 1]);
			}
			m_valid = false;
			break;
		case ':':
			Tracer::cerr("option missing required argument: ", argv[::optind - 1]);
			m_valid = false;
			break;
		}
	}

	if (m_valid) {
		validate();
	}

	if (::optind < argc) {
		while (::optind < argc) {
			Tracer::cerr("unknown argument: ", argv[optind++]);
		}
		set_valid(false);
	}

	if (m_valid) {
		Tracer::set_verbosity(verbosity());
	}
	else {
		usage(argv[0], std::cout);
	}

	return m_valid;
}

void Options::set_real_path(std::string& var, const char* val, const char* opt, const char* name) {
	char buf[PATH_MAX];
	const char* path = ::realpath(val, buf);
	if (path) {
		var = path;
	}
	else {
		int err = errno;
		Tracer::cerr(name, " (", opt, ") ", "invalid path \"", val, "\": ", ::strerror(err));
		m_valid = false;
	}
}

void Options::validate() {
	if (m_template_music_file_set) {
		set_real_path(m_template_music_file, m_template_music_file.c_str(), "-t, --template", "template music file");
	}
	if (m_db_file_set) {
		validate_file("-i, --in", m_db_file_set, "db file", m_db_file.c_str(), R_OK);
		m_num_albums = 1;
		if (m_num_albums_set) {
			Tracer::cerr("ignoring -n ", m_num_albums, " when [-i, --in] is set");
		}
	}
	else {
		validate_dir("-d, --cddb", m_db_dir_set, "db dir", m_db_dir.c_str(), R_OK);
	}
	if (!m_update_cache || m_output_dir_set) {
		validate_dir("-o, --out", m_output_dir_set, "output dir", m_output_dir.c_str(), W_OK);
	}
	if (!m_num_albums_set && m_output_dir_set) {
		m_num_albums = 1;
		m_num_albums_set = true;
	}
	if (!m_db_file_set && m_output_dir_set) {
		validate_num_cds("-n, --num-albums", m_num_albums_set, "num cds", m_num_albums);
	}
	if (validate_file("-t, --template", true, "template music file", m_template_music_file.c_str(), R_OK)) {
		validate_template_music_file();
	}
	if (m_num_threads_set) {
		validate_num_threads();
	}
	if (m_num_threads < 2 || m_db_file_set) {
		m_num_threads = 1;
	}
	m_num_threads = std::min(num_threads(), num_albums());
}

void Options::validate_dir(const char* opt, bool is_set, const char* name, const char* path, int perm) {
	if (!is_set) {
		missing(name, opt);
		return;
	}
	int err = access(path, perm);
	if (err) {
		Tracer::cerr("can't", (perm == R_OK ? " read " : " write "), name, " (", path, "): ", strerror(errno));
		set_valid(false);
		return;
	}
	struct stat sb;
	err = stat(path, &sb);
	if (err) {
		Tracer::cerr("invalid ", name, " (", path, "): ", strerror(err));
		set_valid(false);
		return;
	}
	if (!S_ISDIR(sb.st_mode)) {
		Tracer::cerr(name, " (", path, ") must be a directory");
		set_valid(false);
		return;
	}
}

bool Options::validate_file(const char* opt, bool is_set, const char* name, const char* path, int perm) {
	if (!is_set) {
		missing(name, opt);
		return false;
	}
	int err = access(path, perm);
	if (err) {
		Tracer::cerr("can't", (perm == R_OK ? " read " : " write "), name, " (", path, "): ", strerror(errno));
		set_valid(false);
		return false;
	}
	struct stat sb;
	err = stat(path, &sb);
	if (err) {
		Tracer::cerr("invalid ", name, " (", path, "): ", strerror(err));
		set_valid(false);
		return false;
	}
	if (!S_ISREG(sb.st_mode)) {
		Tracer::cerr(name, " (", path, ") must be a file");
		set_valid(false);
		return false;
	}
	return true;
}

void Options::validate_num_cds(const char* opt, bool is_set, const char* name, int num) {
	if (!is_set) {
		missing(name, opt);
		return;
	}
	bool valid = (num > 0 && num <= MAX_CDS);
	if (!valid) {
		Tracer::cerr(opt, " (", num, ") must be > 0, <= ", MAX_CDS);
	}
	set_valid(valid);
}

void Options::validate_template_music_file() {
	TagLib::FileRef f(m_template_music_file.c_str());
	if (f.isNull()) {
		Tracer::_err("invalid template music file: ", m_template_music_file);
		set_valid(false);
		return;
	}

	auto tag = f.tag();
	if (!tag) {
		Tracer::_err("error in taglib with template file: ", m_template_music_file);
		set_valid(false);
	}
}

void Options::validate_num_threads() {
	if (m_num_threads < 1 || m_num_threads > MAX_THREADS) {
		Tracer::cerr("-c, --threads (", m_num_threads, ") must be > 0, <= ", MAX_THREADS);
		set_valid(false);
	}
}

void Options::missing(const char* name, const char* opt) {
	Tracer::cerr("missing argument ", name, ": ", opt);
	set_valid(false);
}

std::ostream& operator<<(std::ostream& os, const Options& opts) {
	using namespace std;
	os << std::boolalpha;
	os << "is valid: " << opts.m_valid << endl;
	os << "db dir: " << opts.m_db_dir << endl;
	os << "db file: " << opts.m_db_file << endl;
	os << "num cds: " << opts.m_num_albums << endl;
	os << "num threads: " << opts.m_num_threads << endl;
	os << "template mp3: " << opts.m_template_music_file << endl;
	os << "skip empty titles: " << opts.m_skip_empty_titles << endl;
	os << "verbosity: " << opts.m_verbosity << endl;
	return os;
}
} /* namespace fmf */
