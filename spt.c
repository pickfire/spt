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
#define LEN(a)        (sizeof(a) / sizeof(a[0]))

typedef struct {
	unsigned int tmr;
	char *cmt;
} Timers;

#include "config.h"

static int i, timecount;

/* function declarations */
static void die(const char *errstr, ...);
static void spawn(char *);
static void notify_send(char *);
static void remaining_time(int);
static void usage(void);

/* functions implementations */
void
die(const char *errstr, ...)
{
	va_list ap;

	va_start(ap, errstr);
	vfprintf(stderr, errstr, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

void
spawn(char *cmd)
{
	if (fork() == 0) {
		setsid();
		execvp("sh", (char *const []){"/bin/sh", "-c", cmd, 0});
		fprintf(stderr, "spt: execvp %s", cmd);
		perror(" failed");
		exit(0);
	}
}

void
notify_send(char *cmt)
{
	if (!strcmp(notifycmd, "libnotify")) { /* use libnotify */
#ifdef NOTIFY
		notify_init("spt");
		NotifyNotification *n = notify_notification_new("spt", cmt, \
					"dialog-information");
		notify_notification_show(n, NULL);
		g_object_unref(G_OBJECT(n));
		notify_uninit();
#endif /* NOTIFY */
	} else if (strcmp(notifycmd, "")) {
		/* TODO(pickfire): merge this into spawn() */
			setsid();
			execlp(notifycmd, "spt", cmt, NULL);
			fprintf(stderr, "spt: execlp %s", notifycmd);
			perror(" failed");
			exit(0);
	}

	if (strcmp(notifyext, "")) /* extra commands to use */
		spawn(notifyext);
}

void
remaining_time(int sigint)
{
	char remainingtext[17];
	if (signal(SIGUSR1, SIG_IGN) != SIG_IGN) {
		signal(SIGUSR1, remaining_time);
	}

	snprintf(remainingtext, 17, "Remaining: %02d:%02d\n",
		 (timers[i].tmr - timecount) / 60,
		 (timers[i].tmr - timecount) % 60);

	notify_send(remainingtext);
}

void
usage(void)
{
	die("usage: %s [-e notifyext] [-n notifycmd] [-v]\n", argv0);
}

int
main(int argc, char *argv[])
{
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

	if (signal(SIGUSR1, SIG_IGN) != SIG_IGN) {
		signal(SIGUSR1, remaining_time);
	}

run:
	if (fork() == 0) {
		notify_send(timers[i].cmt);
	}

	for (timecount = 0; timecount < timers[i].tmr; timecount++) {
		sleep(1);
	}

	i + 1 >= LEN(timers) ? i = 0 : i++; /* i infinal loop */
	goto run;

	return 0;
}
