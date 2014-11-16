Carpi (maybe Raspberry Radio?)
==============================

Carpi is a project aimed at controlling a large media database
within a car environment. The base of the system is a raspberry
pi which acts as the central database and music player. The music
server is run via mpd. The scripts here should run on any linux
system with python and running systemd. In the instructions here
I focus on arch linux.

## Parts required
* Raspberry Pi (B+ Preferred)
* Wifi Dongle
* USB Storage Device
* Stereo with aux-in
* A wifi device with mpdroid

The Scheme is as follows:
1. Pi Gets power from relay which switches via the stereo's remote wire
2. Pi outputs sound to stereo aux-in (preferably spdif or USB DAC)
3. Pi runs arch linux with mpd
4. The tablet can be controlled via a wiimote (and even play snes)

Reach Goals:
-- Accept phone calls
-- GPS
-- Reverse camera
-- VNC