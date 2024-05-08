/* Timers library code is placed under the MIT license
 * Copyright (c) 2020 Stefan Staub
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Timers.h"

Timers::Timers(resolution_t resolution) {
	this->resolution = resolution;
	}

Timers::~Timers() {}

void Timers::start() {
	timer = 0;
	if(resolution == MILLIS) started = us_ticker_read() / 1000;
	if(resolution == MICROS) started = us_ticker_read();
	status = RUNNING;
	}

void Timers::pause() {
	if (status == RUNNING) {
		if(resolution == MILLIS) timer = timer + us_ticker_read() / 1000 - started;
		if(resolution == MICROS) timer = timer + us_ticker_read() - started;
		status = PAUSED;
		}
	}

void Timers::resume() {
	if (status == PAUSED) {
		if(resolution == MILLIS) started = us_ticker_read() / 1000;
		if(resolution == MICROS) started = us_ticker_read();
		status = RUNNING;
		}
	}

void Timers::stop() {
	if (status == RUNNING) {
		if(resolution == MILLIS) timer = us_ticker_read() / 1000 - started + timer;
		if(resolution == MICROS) timer = us_ticker_read() - started + timer;
		}
	status = STOPPED;
	}

uint32_t Timers::elapsed() {
	if (status == RUNNING) {
		if(resolution == MILLIS) return us_ticker_read() / 1000 - started + timer;
		if(resolution == MICROS) return us_ticker_read() - started + timer;
		}
	return timer;
	}

status_t Timers::state() {
	return status;
	}
