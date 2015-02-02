/*
 * events_windows: event abstraction layer for Windows systems
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

#include "libusbi.h"

int usbi_create_event(usbi_event_t *event)
{
	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!hEvent) {
		usbi_warn(NULL, "failed to create event: %d", GetLastError());
		return LIBUSB_ERROR_OTHER;
	}

	*event = hEvent;
	return 0;
}

int usbi_signal_event(usbi_event_t *event)
{
	if (!SetEvent(*event)) {
		usbi_warn(NULL, "failed to set event: %d", GetLastError());
		return LIBUSB_ERROR_OTHER;
	}

	return 0;
}

int usbi_clear_event(usbi_event_t *event)
{
	if (!ResetEvent(*event)) {
		usbi_warn(NULL, "failed to reset event: %d", GetLastError());
		return LIBUSB_ERROR_OTHER;
	}

	return 0;
}

int usbi_destroy_event(usbi_event_t *event)
{
	if (!CloseHandle(*event)) {
		usbi_warn(NULL, "failed to close event handle: %d", GetLastError());
		return LIBUSB_ERROR_OTHER;
	}

	return 0;
}

usbi_timer_t usbi_create_timer(void)
{
#if !defined(_WIN32_WCE)
	HANDLE hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
	if (!hTimer) {
		usbi_warn(NULL, "failed to create waitable timer: %d", GetLastError());
		return USBI_INVALID_TIMER;
	}

	return hTimer;
#else
	return USBI_INVALID_TIMER;
#endif
}

int usbi_arm_timer(usbi_timer_t timer, struct timeval *tv)
{
#if !defined(_WIN32_WCE)
	LARGE_INTEGER liDueTime;

	/* timeout is specified in 100ns units
	 * negative means relative time */
	liDueTime.QuadPart = (tv->tv_sec * -10000000LL) + (tv->tv_usec * -10LL);

	if (!SetWaitableTimer(timer, &liDueTime, 0, NULL, NULL, FALSE)) {
		usbi_warn(NULL, "failed to set waitable timer: %d", GetLastError());
		return LIBUSB_ERROR_OTHER;
	}

	return 0;
#else
	return LIBUSB_ERROR_NOT_SUPPORTED;
#endif
}

int usbi_disarm_timer(usbi_timer_t timer)
{
#if !defined(_WIN32_WCE)
	/* A manual-reset waitable timer will stay in the signalled state until
	 * another call to SetWaitableTimer() is made. It is possible that the
	 * timer has already expired by the time we come in to disarm it, so to
	 * be entirely sure the timer is disarmed and not in the signalled state,
	 * we will set it with an impossibly large expiration and immediately cancel.
	 */
	struct timeval disarm_tv = { INT32_MAX, 0 };

	if (usbi_arm_timer(timer, &disarm_tv))
		return LIBUSB_ERROR_OTHER;

	if (!CancelWaitableTimer(timer)) {
		usbi_warn(NULL, "failed to cancel waitable timer: %d", GetLastError());
		return LIBUSB_ERROR_OTHER;
	}

	return 0;
#else
	return LIBUSB_ERROR_NOT_SUPPORTED;
#endif
}

int usbi_destroy_timer(usbi_timer_t timer)
{
#if !defined(_WIN32_WCE)
	if (!CloseHandle(timer)) {
		usbi_warn(NULL, "failed to close waitable timer handle: %d", GetLastError());
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
