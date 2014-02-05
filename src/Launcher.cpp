/*
 * Launcher.cpp
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
#include "Launcher.h"
#include "MusicFileCreator.h"
#include "MusicFilesGenerator.h"
#include "Tracer.h"

#include <stddef.h>
#include <exception>
#include <thread>
#include <vector>

namespace FMF {

Launcher::Launcher() {
}

void Launcher::launch(size_t num_threads, MusicFilesGenerator& generator, MusicFileCreator& creator) {
	std::vector<std::thread> threads(num_threads);

	try {
		for (std::thread& t: threads) {
			t = std::thread(std::ref(generator), std::ref(creator));
		}
	}
	catch (std::exception& e) {
		Tracer::_err("failed to launch threads: ", e.what());
	}

	for (std::thread& t: threads) {
		if (t.joinable()) {
			t.join();
		}
	}
}

} /* namespace FMF */
