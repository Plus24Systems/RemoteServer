##----------------------------------------------------------------------------##
##  Application: set_gain.sh for AXIOM Cameras                                ##
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

case $1 in 
   ("1")
        GAIN=0
        ADC_RANGE=0x3eb
    ;;
   ("2")
        GAIN=1
        ADC_RANGE=0x3d5
    ;;
   ("3")
        GAIN=3
        ADC_RANGE=0x3d5  # to be double-checked
    ;;
   ("4")
        GAIN=7
        ADC_RANGE=0x3d5
    ;;
   ("3/3" | "3_3")
        GAIN=11
        ADC_RANGE=0x3e9
    ;;

    (*)
        echo "Usage: $0 <gain>"
        echo "Available gains: 1 2 3 4 3/3"
        exit
    ;;
esac

echo "Setting gain x$1 ($GAIN,$ADC_RANGE)..."
. /root//cmv.func
cmv_reg 115 $GAIN      # gain
cmv_reg 116 $ADC_RANGE # ADC_range fine-tuned for each gain
cmv_reg 100 1          # ADC_range_mult2
cmv_reg 87 2000        # offset 1
cmv_reg 88 2000        # offset 2

