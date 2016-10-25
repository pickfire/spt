/* See LICENSE file for copyright and license details. */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
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

volatile static sig_atomic_t suspend, displaytime;

/* function declarations */
static void die(const char *errstr, ...);
static void spawn(char *cmd, char *cmt);
static void notify_send(char *cmt);
static void remaining_time(int sigint);
static void toggle(int sigint);
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
spawn(char *cmd, char *cmt)
{
	if (fork() == 0) {
		setsid();
		execlp(cmd, cmd, "spt", cmt, NULL);
		die("spt: execlp %s\n", cmd);
		perror(" failed");
		exit(0);
	}
}

void
notify_send(char *cmt)
{
#ifdef NOTIFY
	notify_init("spt");
	NotifyNotification *n = notify_notification_new("spt", cmt, \
				"dialog-information");
	notify_notification_show(n, NULL);
	g_object_unref(G_OBJECT(n));
	notify_uninit();
#else
	if (strcmp(notifycmd, "")) /* TODO: call function in config.h */
		spawn(notifycmd, cmt);
#endif /* NOTIFY */

	if (strcmp(notifyext, "")) /* extra commands to use */
		spawn(notifyext, NULL);
}

void
display_remaining_time(int remainingtime)
{
	char buf[17];

	snprintf(buf, 17, "Remaining: %02d:%02d\n",
		 remainingtime / 60,
		 remainingtime % 60);

	notify_send(buf);
}

void
remaining_time(int sigint)
{
	displaytime = 1;
}

void
toggle(int sigint) {
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
	struct sigaction sa;
	sigset_t emptymask;
	int i, remainingtime;

	ARGBEGIN {
		case 'e':
			notifyext = EARGF(usage());
			break;
		case 'n':
			notifycmd = EARGF(usage());
			break;
		case 'v':
			die("spt " VERSION " © 2015 spt engineers, "
			    "see LICENSE for details\n");
		default:
			usage();
			break;
	} ARGEND;

	/* add SIGUSR1 handler: remaining_time */
	sa.sa_handler = remaining_time;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	if (sigaction(SIGUSR1, &sa, NULL) == -1)
		die("cannot associate SIGUSR1 to handler\n");

	/* add SIGUSR2 handler: toggle */
	sa.sa_handler = toggle;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	if (sigaction(SIGUSR2, &sa, NULL) == -1)
		die("cannot associate SIGUSR2 to handler\n");

	for (i = 0; ; i = (i + 1) % LEN(timers)) {
		notify_send(timers[i].cmt);
		remainingtime = timers[i].tmr;
		while (remainingtime) {
			if (displaytime) {
				display_remaining_time(remainingtime);
				displaytime = 0;
			}

			if (suspend)
				sigsuspend(&emptymask);
			else
				remainingtime = sleep(remainingtime);
		}
	}

	return 0;
}
