#!/bin/bash
#Put this shell script with all the options in "/etc/profile.d/" --> This will modify the properties upon login. 
#Another way to do this is to copy this script at the end of .bashrc file that is on the root folder
echo Modifying camera properties upon startup..
#v4l2-ctl --set-fmt-video=width=160.hieght=120,pixelformat=1
#v4l2-ctl --set-ctrl=focus_auto=0
v4l2-ctl --set-ctrl=zoom_absolute=238
#echo Auto focus turned off and set the zoom
#echo Type "v4l2-ctl -h" for help
#echo ...
#echo Enabling serial communication
#echo enable-uart5 > /sys/devices/bone_capemgr.*/slots
#echo ...

