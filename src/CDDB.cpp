/*
 * CDDB.cpp
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
#include "CDDB.h"
#include "Dir.h"
#include "File.h"
#include "Tracer.h"
#include "Context.h"

#include <dirent.h>
#include <error.h>
#include <string.h>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <numeric>

namespace FMF {

/**
 * thrown when cached cddb dir is different from option cddb dir
 */
class InvalidCacheException: public std::exception {
public:
	InvalidCacheException(const std::string& cddb_dir) :
			m_cached_db_dir(cddb_dir) {
	}
	std::string cached_db_dir() const {
		return m_cached_db_dir;
	}
private:
	std::string m_cached_db_dir;
};

//-----------------------------------------------------------------------------
// CDDB
//-----------------------------------------------------------------------------
CDDB::CDDB(const std::string& db_dir) :
		CDDB(db_dir, s_def_cache_dir, s_def_cache_file) {
}

CDDB::CDDB(const std::string& db_dir, const std::string& cache_dir, const std::string& cache_file_name) :
		m_db_dir(db_dir), m_cache_dir(cache_dir), m_cache_file_name(cache_file_name), m_db_cache(*this), m_prng() {
	auto seed = std::random_device()();
	Tracer::_debug("seeding rng with: ", seed);
	m_prng.seed(seed);
}

CDDB::~CDDB() {
}

std::string CDDB::cddb_cache_file() {
	return cache_dir() + Dir::DIR_SEP + cache_file_name();
}

bool CDDB::init(bool update_cache) {
	bool res = (!update_cache && DBCache::read(m_db_cache, m_cache_dir, m_cache_file_name))
			|| DBCache::create(m_db_cache, s_def_cache_dir, s_def_cache_file);

	if (res && !num_cached_files()) {
		Tracer::_err("the CDDB cache if empty. db dir is probably not a CDDB database dir.");
		res = false;
	}
	return res;
}

std::string CDDB::random_file() const {
	return m_db_cache.random_file(m_prng);
}

size_t CDDB::num_cached_files() const {
	return m_db_cache.size();
}

//-----------------------------------------------------------------------------
// CDDB::DBCache
//-----------------------------------------------------------------------------
CDDB::DBCache::DBCache(const CDDB& cddb) :
		m_cddb(cddb), m_genres(), m_random_dist() {
}

/*static*/bool CDDB::DBCache::create(CDDB::DBCache& cache, const std::string& cache_dir,
		const std::string& cache_file_name) {
	bool success = true;
	const std::string cache_file = cache_dir + Dir::DIR_SEP + cache_file_name;
	Dir dir(cache_dir);
	if (!dir.create()) {
		Tracer::_err("failed to create cache dir: ", cache_dir);
		return false;
	}
	Tracer::cout("creating cddb cache from dir: ", cache.db_dir());
	if (cache.scan()) {
		if (cache.m_genres.size() == 0) {
			Tracer::_err("scanning produced no valid cddb files from dir: ", cache.db_dir());
			return false;
		}
		std::ofstream os(cache_file);
		Tracer::cout("writing cddb cache file: ", cache_file);
		try {
			os << cache;
			cache.init_random();
		}
		catch (std::exception& e) {
			success = false;
			Tracer::_err("writing cache failed: ", cache_file, " ", e.what());
		}
	}
	else {
		if (Context::stopped()) {
			Tracer::cout("cache building interrupted: ", cache.db_dir());
		}
		else {
			Tracer::_err("cache building failed: ", cache.db_dir());
		}
		success = false;
	}
	return success;
}

/*static*/bool CDDB::DBCache::read(CDDB::DBCache& cache, const std::string& cache_dir,
		const std::string& cache_file_name) {
	bool success = true;
	const std::string cache_file = cache_dir + Dir::DIR_SEP + cache_file_name;
	std::ifstream is;
	try {
		is.exceptions(std::ios_base::failbit | std::ios_base::badbit);
		is.open(cache_file);
		is >> cache;
		is.close();
		cache.init_random();
	}
	catch (const InvalidCacheException& e) {
		success = false;
		Tracer::cout("cddb dir changed. was: ", e.cached_db_dir(), " now: ", cache.db_dir());
	}
	catch (const std::exception& e) {
		if (!is.eof()) {
			success = false;
			Tracer::_err("failed reading cache file ", cache_file, " ", e.what());
		}
	}
	return success;
}

Dir::EachResult CDDB::DBCache::on_dir_entry(const Dir& dir, const dirent& de) {
	if (Context::stopped())
		return Dir::EachResult::STOP;
	// ignore genre directory 'data'
	if (de.d_type != DT_DIR || 0 == strcasecmp(de.d_name, "data"))
		return Dir::EachResult::CONTINUE;
	std::cout << "scanning genre dir: " << de.d_name << std::endl;
	Dir sub_dir = dir.sub_dir(de.d_name);
	GenreCache gc(*this, de.d_name);
	if (!sub_dir.for_each(gc))
		return Dir::EachResult::STOP;
	Tracer::_debug(gc.name(), ": ", gc.size());
	if (gc.size() > 0) {
		m_genres.push_back(std::move(gc));
	}
	else {
		Tracer::_warn("scanning produced no valid cddb files from genre dir: ", sub_dir.path());
	}
	return Dir::EachResult::CONTINUE;
}

bool CDDB::DBCache::scan() {
	Dir dir(m_cddb.db_dir());
	return dir.for_each(*this);
}

void CDDB::DBCache::init_random() {
	m_random_dist.param(RandomDistribution::param_type(1, m_genres.size()));
	for (auto& gc : m_genres) {
		gc.init_random();
	}
}

std::string CDDB::DBCache::random_file(RandomGenerator& rand) const {
	return m_genres[m_random_dist(rand) - 1].random_file(rand);
}

std::istream& operator >>(std::istream& is, CDDB::DBCache& cache) {
	std::string db_dir;
	is >> db_dir;
	if (db_dir != cache.db_dir()) {
		throw InvalidCacheException(db_dir);
	}
	size_t num_genres(0);
	is >> num_genres;
	while (num_genres--) {
		std::string name;
		size_t count;
		is >> name >> count;
		CDDB::GenreCache gc(cache, name);
		std::ifstream fs;
		fs.exceptions(std::ios_base::failbit | std::ios_base::badbit);
		fs.open(cache.genre_cache_file(name), std::ios::in | std::ios::binary);
		fs >> gc;
		cache.m_genres.push_back(std::move(gc));
	}
	return is;
}

size_t CDDB::DBCache::size() const {
	return std::accumulate(m_genres.begin(), m_genres.end(), 0,
			[](size_t sum, const GenreCache& genre) {return sum + genre.size();});
}

std::ostream& operator <<(std::ostream& os, const CDDB::DBCache& cache) {
	os << cache.m_cddb.db_dir() << std::endl;
	os << cache.m_genres.size() << std::endl;
	for (auto& gc : cache.m_genres) {
		std::string name = gc.name();
		os << name << " " << gc.size() << std::endl;
		std::ofstream fs(cache.genre_cache_file(name), std::ios::binary);
		fs << gc;
	}
	return os;
}

//-----------------------------------------------------------------------------
// CDDB::GenreCache
//-----------------------------------------------------------------------------
CDDB::GenreCache::GenreCache(DBCache& db_cache, const std::string& dir_name) :
		m_db_cache(db_cache), m_genre_dir_name(dir_name), m_entries(), m_random_dist() {
}

void CDDB::GenreCache::on_dir_begin(const Dir& dir) {
	m_entries.clear();
}

// cddb file names are %08x formatted integers
std::string int_to_file_name(uint32_t n) {
	std::ostringstream os;
	os << std::hex << std::setw(8) << std::setfill('0') << n;
	return os.str();
}

uint32_t file_name_to_int(const char* fname) {
	uint32_t n = 0;
	if (1 != ::sscanf(fname, "%x", &n) || int_to_file_name(n) != fname) {
		n = 0;
	}
	return n;
}

Dir::EachResult CDDB::GenreCache::on_dir_entry(const Dir& dir, const dirent& de) {
	if (Context::stopped())
		return Dir::EachResult::STOP;
	uint32_t n = file_name_to_int(de.d_name);
	if (n) {
		m_entries.push_back(n);
		std::cout << std::setw(12) << m_entries.size() << " " << de.d_name << "\r";
	}
	else {
		Tracer::_err("entry is not hex integer as expected ", de.d_name);
	}
	return Dir::EachResult::CONTINUE;
}

void CDDB::GenreCache::init_random() {
	m_random_dist.param(RandomDistribution::param_type(1, size()));
}

std::string CDDB::GenreCache::random_file(RandomGenerator& rand) const {
	uint32_t n = m_entries.at(m_random_dist(rand) - 1);
	return m_db_cache.db_dir() + Dir::DIR_SEP + name() + Dir::DIR_SEP + int_to_file_name(n);
}

std::istream& operator >>(std::istream& is, CDDB::GenreCache& cache) {
	uint32_t count(0);
	is.read((char*) &count, sizeof(count));
	cache.m_entries.reserve(count);
	while (count--) {
		uint32_t n;
		is.read((char*) &n, sizeof(n));
		cache.m_entries.push_back(n);
	}
	return is;
}

std::ostream& operator <<(std::ostream& os, const CDDB::GenreCache& cache) {
	uint32_t n = cache.m_entries.size();
	os.write((char*) &n, sizeof(n));
	os.write((char*) cache.m_entries.begin().base(), cache.m_entries.size() * sizeof(uint32_t));
	return os;
}

}/* namespace FMF */
