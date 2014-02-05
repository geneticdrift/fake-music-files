/*
 * main.cpp
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
#include "Launcher.h"
#include "MusicFileCreator.h"
#include "MusicFilesGenerator.h"
#include "Options.h"
#include "Tracer.h"

using FMF::Context;
using FMF::Launcher;
using FMF::MusicFilesGenerator;
using FMF::MusicFileCreator;
using FMF::Options;
using FMF::Tracer;

#include <cstdlib>
#include <stdlib.h>
#include <iostream>
#include <future>
#include <signal.h>

void sig_handler(int sig) {
	Context::signal();
}

/**
 * @mainpage Fake music files
 */
int main(int argc, char** argv) {
	::sigset_t sig_mask;
	::sigemptyset(&sig_mask);
	struct sigaction sa;
	sa.sa_handler = sig_handler;
	sa.sa_mask = sig_mask;
	sa.sa_flags = 0;
	sa.sa_restorer = 0;
	::sigaction(SIGINT, &sa, 0);
	::sigaction(SIGQUIT, &sa, 0);
	::sigaction(SIGHUP, &sa, 0);

	Options opts;

	if (!opts.parse(argc, argv))
		return EXIT_FAILURE;

	Context ctx(opts);

	if (!ctx.init())
		return EXIT_FAILURE;

	MusicFileCreator creator(ctx);

	MusicFilesGenerator generator(ctx);

	if (opts.num_threads() > 1) {
		Launcher launcher;
		launcher.launch(opts.num_threads(), generator, creator);
	}
	else {
		generator(creator);
	}

	ctx.output_summary(std::cout);

	return EXIT_SUCCESS;
}

