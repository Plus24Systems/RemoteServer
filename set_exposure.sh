#!/bin/sh

##----------------------------------------------------------------------------##
##  Application: set_exposure.sh for AXIOM Cameras                            ##
##                                                                            ##
##  Author:   Phil Kerr                                                       ##
##  Company:  Plus24 Systems Ltd.                                             ##
##  GitHub:   https://github.com/Plus24Systems/RemoteServer                   ##
##  Copyright (C) 2016 - 2017 Phil Kerr - Plus24 Systems Ltd.                 ##
##                                                                            ##
##   This program is free software: you can redistribute it and/or modify     ##
##   it under the terms of the GNU General Public License as published by     ##
##   the Free Software Foundation, either version 3 of the License, or        ##
##   (at your option) any later version.                                      ##
##                                                                            ##
##   This program is distributed in the hope that it will be useful,          ##
##   but WITHOUT ANY WARRANTY; without even the implied warranty of           ##
##   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            ##
##   GNU General Public License for more details.                             ##
##                                                                            ##
##   You should have received a copy of the GNU General Public License        ##
##   along with this program.  If not, see <http:##www.gnu.org/licenses/>.    ##
##                                                                            ##
##                                                                            ##
##----------------------------------------------------------------------------##

case $1 in 
    ("0")
        EXPOSURE=5ms
    ;;
    ("1")
        EXPOSURE=10ms
    ;;
    ("2")
        EXPOSURE=15ms
    ;;
    ("3")
        EXPOSURE=20ms
    ;;
    ("4")
        EXPOSURE=25ms
    ;;
    ("5")
        EXPOSURE=30ms
    ;;
    ("6")
        EXPOSURE=35ms
    ;;    
    ("7")
        EXPOSURE=40ms
    ;; 

    (*)
        echo "Usage: $0 <exposure>"
        echo "Available exposure: 0 1 2 3 4 5 6 7"
        echo "                    0 = 5ms"
        echo "                    1 = 10ms"
        echo "                    2 = 15ms"
        echo "                    3 = 20ms"
        echo "                    4 = 25ms"
        echo "                    5 = 30ms"
        echo "                    6 = 35ms"
        echo "                    7 = 40ms"
        exit
    ;;
esac

echo "Setting exposure $EXPOSURE"
. /root/cmv.func
/root/cmv_snap3 -z -e $EXPOSURE

