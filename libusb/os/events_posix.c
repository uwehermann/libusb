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

#include "libusbi.h"

int usbi_create_event(usbi_event_t *event)
{
	return LIBUSB_ERROR_NOT_SUPPORTED;
}

int usbi_signal_event(usbi_event_t *event)
{
	return LIBUSB_ERROR_NOT_SUPPORTED;
}

int usbi_clear_event(usbi_event_t *event)
{
	return LIBUSB_ERROR_NOT_SUPPORTED;
}

int usbi_destroy_event(usbi_event_t *event)
{
	return LIBUSB_ERROR_NOT_SUPPORTED;
}

usbi_timer_t usbi_create_timer(void)
{
	return USBI_INVALID_TIMER;
}

int usbi_arm_timer(usbi_timer_t timer, struct timeval *tv)
{
	return LIBUSB_ERROR_NOT_SUPPORTED;
}

int usbi_disarm_timer(usbi_timer_t timer)
{
	return LIBUSB_ERROR_NOT_SUPPORTED;
}

int usbi_destroy_timer(usbi_timer_t timer)
{
	return LIBUSB_ERROR_NOT_SUPPORTED;
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
