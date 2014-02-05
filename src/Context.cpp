/*
 * Context.cpp
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
#include "Context.h"
#include "Tracer.h"

#include <algorithm>
#include <ios>
#include <vector>
#include <signal.h>

namespace FMF {

/*static*/bool Context::s_signaled = false;

Context::Context(const Options& opts) :
		m_opts(opts), m_cddb(nullptr), m_parse_counts_mutex(), m_parse_success(0), m_parse_pending(0), m_parse_failed(
				0), m_parse_failed_files(), m_create_success(0), m_create_failed(0), m_create_skipped(0) {
}

Context::~Context() {
	delete m_cddb;
}

bool Context::init() {
	if (m_opts.is_db_dir_set()) {
		m_cddb = new (std::nothrow) CDDB(m_opts.db_dir());
		return m_cddb && m_cddb->init(m_opts.update_cache());
	}
	return true;
}

bool Context::pick_db_file(std::string& db_file_path) {
	std::lock_guard < std::mutex > locker(m_parse_counts_mutex);
	if (m_parse_success + m_parse_pending >= m_opts.num_albums() || m_parse_failed >= m_opts.num_albums()) {
		return false;
	}
	db_file_path = m_opts.is_db_file_set() ? m_opts.db_file() : m_cddb->random_file();
	m_parse_pending += 1;
	return true;
}

size_t Context::on_parse_success() {
	std::lock_guard < std::mutex > locker(m_parse_counts_mutex);
	m_parse_success += 1;
	m_parse_pending -= 1;
	return m_parse_success;
}

size_t Context::on_parse_failed(const std::string& db_file) {
	std::lock_guard < std::mutex > locker(m_parse_counts_mutex);
	m_parse_failed += 1;
	m_parse_pending -= 1;
	m_parse_failed_files.push_back(db_file);
	return m_parse_failed;
}

std::ostream& FMF::Context::output_summary(std::ostream& os) const {
	enum {
		parsed, created, parse_failed, create_failed
	};
	std::vector<std::pair<std::string, size_t>> titles {
															std::make_pair("Parsed CDDB files", parse_success_count()),
															std::make_pair("Created fake music files",
																	create_success_count()),
															std::make_pair("Skipped exisiting fake music files",
																	create_skipped_count()),
															std::make_pair("Failed parse CDDB files",
																	parse_fail_count()),
															std::make_pair("Failed fake music files",
																	create_failed_count()) };

	const size_t max_title_len = std::max_element(titles.begin(), titles.end(),
			[](const std::pair<std::string, size_t>& t1, const std::pair<std::string, size_t>& t2) {
				return t1.first.size() < t2.first.size();})->first.size();

	const size_t max_num_len = 10;

	os << std::setw(max_num_len + max_title_len) << std::setfill('-') << "" << std::endl;
	if (Context::stopped()) {
		os << "Stopped by signal" << std::endl;
	}
	os << "Results:" << std::endl;
	os << std::setw(max_num_len + max_title_len) << std::setfill('-') << "" << std::endl;

	for (auto& pair : titles) {
		os << std::resetiosflags(std::ios::right) << std::setfill('.') << std::setw(max_title_len) << std::left
				<< pair.first << std::setw(max_num_len) << std::right << pair.second << std::endl;
	}

	if (m_parse_failed) {
		os << std::setw(max_num_len + max_title_len) << std::setfill('-') << "" << std::endl;
		os << "Failed to parse these db files:" << std::endl;
		for (auto& fpath : m_parse_failed_files)
			os << fpath << std::endl;
		os << std::setw(max_num_len + max_title_len) << std::setfill('-') << "" << std::endl;

	}
	return os;
}

} /* namespace fmf */

