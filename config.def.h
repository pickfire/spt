/* See LICENSE file for copyright and license details. */

/* Notification */
static char *notifycmd = "libnotify"; /* Use libnotify or given command */
static char *notifyext = ""; /* Notify with extra command (eg. play an alarm) */

/*
 * This is the array which defines all the timer that will be used.
 * It will be repeated after all of it is executed.
 */
static Timers timers[] = {
	/* timer(s)  comment */
	{     1500,  "Time to start working!"},
	{      300,  "Time to start resting!"},
	{     1500,  "Time to start working!"},
	{      300,  "Time to start resting!"},
	{     1500,  "Time to start working!"},
	{      300,  "Time to start resting!"},
	{     1500,  "Time to start working!"},
	{      300,  "Time to start resting!"},
	{      900,  "Time to take some nap!"},
};
