##----------------------------------------------------------------------------##
##  Application: set_hdmi_live_stream.sh for AXIOM Cameras                    ##
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
   ("ON")
        REG15=0x01000100
    ;;
   ("OFF")
        REG15=0
    ;;

    (*)
        echo "Usage: $0 <gain>"
        echo "Available values: ON or OFF"
        exit
    ;;
esac


echo "Setting gain $1"

. /root/cmv.func
/root/cmv_snap3 -z -e $EXPOSURE
