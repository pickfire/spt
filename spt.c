/* See LICENSE file for copyright and license details. */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#ifdef NOTIFY
#include <libnotify/notify.h>
#endif /* NOTIFY */

#include "arg.h"

char *argv0;

/* macros */
#define LEN(a)  (sizeof(a) / sizeof(a[0]))


typedef struct {
	unsigned int tmr;
	char *cmt;
} Timers;

#include "config.h"

volatile static sig_atomic_t display, suspend;

/* function declarations */
static void die(const char *errstr, ...);
static void spawn(char *argv[]);
static void notify_send(char *cmt);
static void display_state(int remaining, int suspend);
static void toggle_display(int sigint);
static void toggle_suspend(int sigint);
static void usage(void);

/* functions implementations */
void
die(const char *errstr, ...)
{
	va_list ap;

	va_start(ap, errstr);
	vfprintf(stderr, errstr, ap);
	va_end(ap);
	exit(1);
}

void
spawn(char *argv[])
{
	if (fork() == 0) {
		setsid();
		execvp(argv[0], argv);
		die("spt: execvp %s\n", argv[0]);
		perror(" failed");
		exit(0);
	}
}

void
notify_send(char *cmt)
{
	if (strcmp(notifycmd, ""))
		spawn((char *[]) { notifycmd, "spt", cmt, NULL });
#ifdef NOTIFY
	else {
		notify_init("spt");
		NotifyNotification *n = notify_notification_new("spt", cmt, \
					"dialog-information");
		notify_notification_show(n, NULL);
		g_object_unref(G_OBJECT(n));
		notify_uninit();
	}
#endif /* NOTIFY */

	if (strcmp(notifyext, "")) /* extra commands to use */
		spawn((char *[]) { "/bin/sh", "-c", notifyext, NULL });
}

void
display_state(int remaining, int suspend)
{
	char buf[29];

	snprintf(buf, 29, "Remaining: %02d:%02d %s",
		 remaining / 60,
		 remaining % 60,
		 (suspend) ? "◼" : "▶");

	notify_send(buf);
	display = 0;
}

void
toggle_display(int sigint)
{
	display = 1;
}

void
toggle_suspend(int sigint)
{
	suspend ^= 1;
}

void
usage(void)
{
	die("usage: %s [-e notifyext] [-n notifycmd] [-v]\n", argv0);
}

int
main(int argc, char *argv[])
{
	struct timespec remaining;
	struct sigaction sa;
	sigset_t emptymask;
	int i;

	ARGBEGIN {
		case 'e':
			notifyext = EARGF(usage());
			break;
		case 'n':
			notifycmd = EARGF(usage());
			break;
		case 'v':
			die("spt " VERSION " © 2015-2020 spt engineers, "
			    "see LICENSE for details\n");
		default:
			usage();
	} ARGEND;

	/* add SIGUSR1 handler: remaining_time */
	sa.sa_handler = toggle_display;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	if (sigaction(SIGUSR1, &sa, NULL) == -1)
		die("cannot associate SIGUSR1 to handler\n");

	/* add SIGUSR2 handler: toggle */
	sa.sa_handler = toggle_suspend;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	if (sigaction(SIGUSR2, &sa, NULL) == -1)
		die("cannot associate SIGUSR2 to handler\n");

	sigemptyset(&emptymask);

	for (i = 0; ; i = (i + 1) % LEN(timers)) {
		notify_send(timers[i].cmt);
		remaining.tv_sec = timers[i].tmr;
		remaining.tv_nsec = 0;
		while (remaining.tv_sec) {
			if (display)
				display_state(remaining.tv_sec, suspend);

			if (suspend)
				sigsuspend(&emptymask);
			else
				if (nanosleep(&remaining, &remaining) == 0)
					remaining.tv_sec = remaining.tv_nsec = 0;
		}
	}

	return 0;
}
