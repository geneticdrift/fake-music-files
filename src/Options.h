/*
 * Options.h
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
#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <string>
#include <stdlib.h>
#include <getopt.h>
#include <iosfwd>

namespace FMF {

/**
 * command line options
 */
class Options {
public:
	Options();
	~Options();

	bool parse(int argc, char** argv);

	const std::string& db_dir() const {
		return m_db_dir;
	}

	bool is_db_dir_set() const {
		return m_db_dir_set;
	}

	const std::string& db_file() const {
		return m_db_file;
	}

	bool is_db_file_set() const {
		return m_db_file_set;
	}

	bool is_num_albums_set() const {
		return m_num_albums_set;
	}

	size_t num_albums() const {
		return m_num_albums;
	}

	bool is_output_dir_set() const {
		return m_output_dir_set;
	}

	const std::string& output_dir() const {
		return m_output_dir;
	}

	/**
	 * @return the template music file set by option -t
	 */
	const std::string& template_music_file() const {
		return m_template_music_file;
	}

	size_t verbosity() const {
		return m_verbosity;
	}

	bool skip_empty_titles() const {
		return m_skip_empty_titles;
	}

	bool update_cache() const {
		return m_update_cache;
	}

	size_t num_threads() const {
		return m_num_threads;
	}

	friend std::ostream& operator<<(std::ostream& os, const Options& opts);

private:
	std::string m_db_dir;
	std::string m_output_dir;
	size_t m_num_albums;
	std::string m_db_file;
	std::string m_template_music_file;
	bool m_skip_empty_titles;
	size_t m_verbosity;
	bool m_update_cache;
	size_t m_num_threads;

	static const int MAX_CDS;
	static const int MAX_THREADS;
	static const char* DEFAULT_TEMPLATE;

	static struct option s_options[];
	void validate();
	void validate_dir(const char* opt, bool is_set, const char* name,
			const char* var, int perm);
	bool validate_file(const char* opt, bool is_set, const char* name,
			const char* var, int perm);
	void validate_num_cds(const char* opt, bool is_set, const char* name,
			int num);
	void validate_template_music_file();
	void validate_num_threads();
	void set_valid(bool valid) {
		m_valid = m_valid && valid;
	}
	void missing(const char* name, const char* opt);
	void set_real_path(std::string& var, const char* val, const char* opt,
			const char* name);
	void usage(const char* prog_name, std::ostream& os);

	bool m_valid;
	bool m_output_dir_set;
	bool m_db_dir_set;
	bool m_num_albums_set;
	bool m_db_file_set;
	bool m_template_music_file_set;
	bool m_num_threads_set;
};

} /* namespace fmf */
#endif /* OPTIONS_H_ */
