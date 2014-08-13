/*
 * Context.h
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
#ifndef CONTEXT_H_
#define CONTEXT_H_

#include "Options.h"
#include "CDDB.h"
#include "Tracer.h"

#include <atomic>
#include <mutex>
#include <vector>
#include <iomanip>
#include <iosfwd>
#include <ostream>

namespace FMF {

/**
 * application context
 */
class Context {
public:
	Context(const Options& opts);

	~Context();

	bool init();

	bool pick_db_file(std::string& db_file_path);

	size_t on_parse_success();

	size_t on_parse_failed(const std::string& db_file);

	size_t on_create_success() {
		int output_interval = m_opts.num_albums() > 200 ? 1000 : 100;
		if (!(++m_create_success % output_interval)) {
			Tracer::cout("Created: ", m_create_success);
		}
		return m_create_success;
	}

	size_t on_create_failed() {
		return ++m_create_failed;
	}

	size_t on_create_skipped() {
		return ++m_create_skipped;
	}

	size_t parse_success_count() const {
		return m_parse_success;
	}

	size_t parse_fail_count() const {
		return m_parse_failed;
	}

	size_t create_success_count() const {
		return m_create_success;
	}

	size_t create_failed_count() const {
		return m_create_failed;
	}

	size_t create_skipped_count() const {
		return m_create_skipped;
	}

	const Options& options() const {
		return m_opts;
	}

	std::ostream& output_summary(std::ostream& os) const;

	static bool stopped() {
		return s_signaled;
	}

	static void signal() {
		s_signaled = true;
	}

private:
	const Options& m_opts;
	CDDB* m_cddb;

	std::mutex m_parse_counts_mutex;
	size_t m_parse_success;
	size_t m_parse_pending;
	size_t m_parse_failed;

	std::vector<std::string> m_parse_failed_files;

	std::atomic<size_t> m_create_success;
	std::atomic<size_t> m_create_failed;
	std::atomic<size_t> m_create_skipped;

	static bool s_signaled;
};

} /* namespace fmf */
#endif /* CONTEXT_H_ */
