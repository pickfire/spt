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

static int i, timecount, increment;

/* function declarations */
static void die(const char *errstr, ...);
static void spawn(char *cmd, char *cmt);
static void notify_send(char *cmt);
static void remaining_time(int sigint);
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
remaining_time(int sigint)
{
	char buf[17];
	//if (signal(SIGUSR1, SIG_IGN) != SIG_IGN)
	//	signal(SIGUSR1, remaining_time);

	snprintf(buf, 17, "Remaining: %02d:%02d\n",
		  (timers[i].tmr - timecount) / 60,
		  (timers[i].tmr - timecount) % 60);

	notify_send(buf);
}

void 
toggle_pause(int sigint) {
  	//if (increment) {
  	//  notify_send("Paused");
  	//  increment = 0;
  	//else {
  	//  notify_send("Resumed");
  	//  increment = 1;
  	//}
      	increment = increment ? 0 : 1 ;
}

void
usage(void)
{
	die("usage: %s [-e notifyext] [-n notifycmd] [-v]\n", argv0);
}

int
main(int argc, char *argv[])
{
	increment = 1;

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

	if (signal(SIGUSR1, SIG_IGN) != SIG_IGN)
		signal(SIGUSR1, remaining_time);
	if (signal(SIGUSR2, SIG_IGN) != SIG_IGN)
		signal(SIGUSR2, toggle_pause);

run:
	notify_send(timers[i].cmt);

	for (timecount = 0; timecount < timers[i].tmr; timecount += increment)
		sleep(1);

	i = i < LEN(timers) ? i+1 : 0; /* i infinal loop */
	goto run;

	return 0;
}
