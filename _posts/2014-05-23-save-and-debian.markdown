---
layout: post
title: Saving Settings and Debian/Ubuntu Release

---


Two bits of news.

QTimeLapse now has the ability to save settings to a file.  While this will
eventually grow to encompass camera settings as well, for now that means that
all settings pertaining to the timelapse such as interval and max frames can be
persisted to a file that can be opened again later to continue running the same
timelapse again.

Additionally, I finally got the .deb built for this project.  This means that
you can now install on Ubuntu and Debian by grabbing the .deb from
[here](https://launchpad.net/~makyo/+archive/qtimelapse).  If you're on Ubuntu
Trusty or later, you can add the PPA to your system and install like so:

    sudo apt-add-repository ppa:makyo/qtimelapse && sudo apt-get update
    sudo apt-get install qtimelapse

Future work already underway is a Mac OS X build, which will be available from
the project's [Launchpad page](https://launchpad.net/qtl).  Windows will come
soon after.
