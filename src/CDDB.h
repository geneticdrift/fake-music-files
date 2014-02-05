/*
 * CDDB.h
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
#ifndef FREEDB_H_
#define FREEDB_H_

#include "Dir.h"

#include <string>
#include <vector>
#include <time.h>
#include <iosfwd>
#include <random>
#include <cstdint>

namespace FMF {

typedef std::default_random_engine RandomGenerator;
typedef std::uniform_int_distribution<int> RandomDistribution;

/**
 * free cddb directory with sub dir for each of 11 genres and files for each cd.
 *
 * http://www.freedb.org/en/download__database.10.html
 */
class CDDB {
public:
	CDDB(const std::string& db_dir);
	CDDB(const std::string& db_dir, const std::string& cache_dir, const std::string& cache_file_name);
	~CDDB();

	/**
	 * init the cddb
	 *
	 * @param update_cache  if true, update the cache unconditionally
	 *
	 * @return true if successful, false if failed
	 */
	bool init(bool update_cache = false);

	/**
	 * get the path for a random cddb file from a random genre.
	 *
	 * @return path of random cddb file
	 */
	std::string random_file() const;

	size_t num_cached_files() const;

private:
	class DBCache;
	friend std::istream& operator>>(std::istream& is, DBCache& cache);
	friend std::ostream& operator<<(std::ostream& os, const DBCache& cache);

	class GenreCache;
	friend std::istream& operator>>(std::istream& is, GenreCache& cache);
	friend std::ostream& operator<<(std::ostream& os, const GenreCache& cache);

	const std::string& db_dir() const {
		return m_db_dir;
	}

	const std::string& cache_dir() const {
		return m_cache_dir;
	}

	const std::string& cache_file_name() const {
		return m_cache_file_name;
	}

	class DBCache;

	/**
	 * access to files in a genre sub directory of cddb directory
	 */
	struct GenreCache: Dir::DirScanHandler {
		GenreCache(DBCache& db_cache, const std::string& dir_name);

		virtual void on_dir_begin(const Dir& dir);
		virtual Dir::EachResult on_dir_entry(const Dir& dir, const dirent& de);
		virtual void on_dir_end(const Dir& dir) {
		}

		void init_random();

		std::string random_file(RandomGenerator& rand) const;

		const std::string& name() const {
			return m_genre_dir_name;
		}

		size_t size() const {
			return m_entries.size();
		}

		friend std::istream& operator>>(std::istream& is, GenreCache& cache);
		friend std::ostream& operator<<(std::ostream& os, const GenreCache& cache);

	private:
		DBCache& m_db_cache;
		std::string m_genre_dir_name;
		std::vector<uint32_t> m_entries;
		mutable RandomDistribution m_random_dist;
	};

	/**
	 * access to the sub directories of the cddb directory
	 */
	struct DBCache: Dir::DirScanHandler {
		DBCache(const CDDB& cddb);

		static bool create(CDDB::DBCache& cache, const std::string& cache_dir, const std::string& cache_file);

		static bool read(CDDB::DBCache& cache, const std::string& cache_dir, const std::string& cache_file);

		virtual void on_dir_begin(const Dir& dir) {
		}
		virtual Dir::EachResult on_dir_entry(const Dir& dir, const dirent& de);
		virtual void on_dir_end(const Dir& dir) {
		}

		bool scan();

		void init_random();

		std::string random_file(RandomGenerator& rand) const;

		size_t size() const;

		friend std::istream& operator>>(std::istream& is, DBCache& cache);
		friend std::ostream& operator<<(std::ostream& os, const DBCache& cache);

	private:
		friend class GenreCache;

		std::string genre_cache_file(const std::string& name) const {
			return cache_dir() + Dir::DIR_SEP + cache_file_name() + '.' + name;
		}

		const std::string& db_dir() const {
			return m_cddb.db_dir();
		}

		const std::string& cache_dir() const {
			return m_cddb.cache_dir();
		}

		const std::string& cache_file_name() const {
			return m_cddb.cache_file_name();
		}

		const CDDB& m_cddb;
		std::vector<GenreCache> m_genres;
		mutable RandomDistribution m_random_dist;
	};

	std::string cddb_cache_file();

	static constexpr const char* s_def_cache_file = "cddb.cache";
	static constexpr const char* s_def_cache_dir = "fmf.cache";

	const std::string m_db_dir;
	const std::string m_cache_dir;
	const std::string m_cache_file_name;
	DBCache m_db_cache;
	mutable RandomGenerator m_prng;
};

}/* namespace FMF */
#endif /* FREEDB_H_ */
