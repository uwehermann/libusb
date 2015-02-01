#ifndef LIBUSB_EVENTS_POSIX_H
#define LIBUSB_EVENTS_POSIX_H

#include <poll.h>

#define USBI_OS_HANDLE_DESC		"fd"
#define USBI_OS_HANDLE_FORMAT_SPECIFIER	"%d"
#define USBI_EVENT_MASK			POLLIN

typedef struct {
        int fd[2];
} usbi_event_t;

typedef int usbi_timer_t;

#define USBI_EVENT_GET_SOURCE(event)	((event).fd[0])
#define USBI_INVALID_TIMER		-1

#endif /* LIBUSB_EVENTS_POSIX_H */
