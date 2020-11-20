spt - simple pomodoro timer
===========================
spt is a simple timer that uses the pomodoro technique that doubles your
efficiency.

Features
--------
- Get the jobs done quicker than ever
- Keeps you free like a dog
- Able to show remaining time

Installation
------------
Edit config.mk to match your local setup (spt is installed into the /usr/local
namespace by default).

Afterwards enter the following command to build and install spt (if necessary
as root):

    make clean install

See the man pages for additional details.

Configuration
-------------
The configuration of spt is done by creating a custom config.h and
(re)compiling the source code. By default, the timer runs by 4
pomodoro timer (25 mins) with subsequent rests in between (5 mins)
followed by a long rest (15 mins) in an infinite loop.

Links
-----
http://pomodorotechnique.com/


The project is licensed under the MIT license.
