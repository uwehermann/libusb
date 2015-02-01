/*
 * events_posix: event abstraction layer for POSIX systems
 * Copyright © 2015 Chris Dickens <christopher.a.dickens@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include <config.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#ifdef USBI_USING_EVENTFD
#include <sys/eventfd.h>
#endif
#ifdef USBI_USING_TIMERFD
#include <sys/timerfd.h>
#endif

#include "libusbi.h"

#ifdef USBI_USING_EVENTFD
#define EVENT_READ_FD(event)	(*(event))
#define EVENT_WRITE_FD(event)	(*(event))
#else
#define EVENT_READ_FD(event)	((event)->fd[0])
#define EVENT_WRITE_FD(event)	((event)->fd[1])
#endif


int usbi_create_event(usbi_event_t *event)
{
	int r;

#ifdef USBI_USING_EVENTFD
	r = eventfd(0, EFD_NONBLOCK);
	if (r == -1) {
		usbi_warn(NULL, "failed to create eventfd: %d", errno);
		return LIBUSB_ERROR_OTHER;
	}

	*event = r;
	return 0;
#else
	r = pipe(event->fd);
	if (r == -1) {
		usbi_warn(NULL, "failed to create internal pipe: %d", errno);
		return LIBUSB_ERROR_OTHER;
	}
	r = fcntl(event->fd[1], F_GETFL);
	if (r == -1) {
		usbi_warn(NULL, "failed to get pipe fd flags: %d", errno);
		goto err_close_pipe;
	}
	r = fcntl(event->fd[1], F_SETFL, r | O_NONBLOCK);
	if (r == -1) {
		usbi_warn(NULL, "failed to set non-blocking write on new pipe: %d", errno);
		goto err_close_pipe;
	}

	return 0;

err_close_pipe:
	close(event->fd[0]);
	close(event->fd[1]);
	return LIBUSB_ERROR_OTHER;;
#endif
}

int usbi_signal_event(usbi_event_t *event)
{
	uint64_t dummy = 1;
	ssize_t r;

	r = write(EVENT_WRITE_FD(event), &dummy, sizeof(dummy));
	if (r == -1) {
		usbi_warn(NULL, "internal signalling write failed: %d", errno);
		return LIBUSB_ERROR_IO;
	}

	return 0;
}

int usbi_clear_event(usbi_event_t *event)
{
	uint64_t dummy;
	ssize_t r;

	r = read(EVENT_READ_FD(event), &dummy, sizeof(dummy));
	if (r == -1) {
		usbi_warn(NULL, "internal signalling read failed: %d", errno);
		return LIBUSB_ERROR_IO;
	}

	return 0;
}

int usbi_destroy_event(usbi_event_t *event)
{
#ifdef USBI_USING_EVENTFD
	int r = close(*event);
	if (r == -1) {
		usbi_warn(NULL, "failed to close eventfd: %d", errno);
		return LIBUSB_ERROR_OTHER;
	}
#else
	int r1, r2;

	r1 = close(event->fd[0]);
	if (r1 == -1)
		usbi_warn(NULL, "internal pipe close (read) failed: %d", errno);

	r2 = close(event->fd[1]);
	if (r2 == -1)
		usbi_warn(NULL, "internal pipe close (write) failed: %d", errno);

	if (r1 == -1 || r2 == -1)
		return LIBUSB_ERROR_OTHER;
#endif

	return 0;
}

usbi_timer_t usbi_create_timer(void)
{
#ifdef USBI_USING_TIMERFD
	static clockid_t clockid = -1;
	int timerfd;

	if (clockid == -1) {
#ifdef CLOCK_MONOTONIC
		struct timespec ts;
		int r;

		r = clock_gettime(CLOCK_MONOTONIC, &ts);
		if (r == 0)
		{
			clockid = CLOCK_MONOTONIC;
		}
		else
#endif
		{
			clockid = CLOCK_REALTIME;
		}
	}

	timerfd = timerfd_create(clockid, TFD_NONBLOCK);
	if (timerfd == -1) {
		usbi_warn(NULL, "failed to create timerfd: %d", errno);
		return USBI_INVALID_TIMER;
	}

	return timerfd;
#else
	return USBI_INVALID_TIMER;
#endif
}

int usbi_arm_timer(usbi_timer_t timer, struct timeval *tv)
{
#ifdef USBI_USING_TIMERFD
	int r;
	const struct itimerspec it = { { 0, 0 },
		{ tv->tv_sec, (tv->tv_usec * 1000) } };
	r = timerfd_settime(timer, 0, &it, NULL);
	if (r == -1) {
		usbi_warn(NULL, "failed to arm timerfd: %d", errno);
		return LIBUSB_ERROR_OTHER;
	}

	return 0;
#else
	return LIBUSB_ERROR_NOT_SUPPORTED;
#endif
}

int usbi_disarm_timer(usbi_timer_t timer)
{
#ifdef USBI_USING_TIMERFD
	int r;
	const struct itimerspec disarm_timer = { { 0, 0 }, { 0, 0 } };
	r = timerfd_settime(timer, 0, &disarm_timer, NULL);
	if (r == -1) {
		usbi_warn(NULL, "failed to disarm timerfd: %d", errno);
		return LIBUSB_ERROR_OTHER;
	}

	return 0;
#else
	return LIBUSB_ERROR_NOT_SUPPORTED;
#endif
}

int usbi_destroy_timer(usbi_timer_t timer)
{
#ifdef USBI_USING_TIMERFD
	int r = close(timer);
	if (r == -1) {
		usbi_warn(NULL, "failed to close timerfd: %d", errno);
		return LIBUSB_ERROR_OTHER;
	}

	return 0;
#else
	return LIBUSB_ERROR_NOT_SUPPORTED;
#endif
}

int usbi_alloc_event_data(struct libusb_context *ctx)
{
	return LIBUSB_ERROR_OTHER;
}

int usbi_handle_events(struct libusb_context *ctx, void *event_data, unsigned int cnt,
	unsigned int internal_cnt, int timeout_ms)
{
	return LIBUSB_ERROR_OTHER;
}
