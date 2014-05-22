qtimelapse
==========

Qt-based time-lapse software.

Installing
----------

### Ubuntu

QTimeLapse is packaged for Ubuntu 14.04 Trusty and later and available in a PPA:

    sudo apt-add-repository ppa:makyo/qtimelapse && sudo apt-get update
    sudo apt-get install qtimelapse

Development Prerequisites
-------------------------

Although incomplete, this software currently requires:

* Qt Creator - to build
* libgphoto2 - for gphoto
* mencoder *optional* - to generate time-lapse videos from images

Credits
-------

This was heavily based off Tim Nugent's excellent gtimelapse.
