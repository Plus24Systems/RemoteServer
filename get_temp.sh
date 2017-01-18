#!/bin/sh

##----------------------------------------------------------------------------##
##  Application: set_temp.sh for AXIOM Cameras                                ##
##                                                                            ##
##  Author:   Phil Kerr                                                       ##
##  Company:  Plus24 Systems Ltd.                                             ##
##  GitHub:   https://github.com/Plus24Systems/RemoteServer                   ##
##  Copyright (C) 2016 Phil Kerr - Plus24 Systems Ltd.                        ##
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

cd "${0%/*}"            # change into script dir

ZTO=`cat /sys/devices/soc0/amba/*.adc/iio*/in_temp0_offset`
ZTR=`cat /sys/devices/soc0/amba/*.adc/iio*/in_temp0_raw`
ZTS=`cat /sys/devices/soc0/amba/*.adc/iio*/in_temp0_scale`

ZT=`dc -e "5k $ZTR ${ZTO/-/_} + $ZTS * 1000 / p"`

if [ $# -eq 1 ]; then
    if [ "$1" == "-s" ]; then
        printf "%-14.14s\n" $ZT
    fi
fi

if [ $# -eq 0 ]; then
    printf "%-14.14s\t%6.4f °C\n" "ZYNQ Temp" $ZT
fi
