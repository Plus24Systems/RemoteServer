##----------------------------------------------------------------------------##
##  Application: set_exposure.sh for AXIOM Cameras                            ##
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
   ("5")
        EXPOSURE=5ms
    ;;
   ("10")
        EXPOSURE=10ms
    ;;
   ("15")
        EXPOSURE=15ms
    ;;
   ("20")
        EXPOSURE=20ms
    ;;
   ("25")
        EXPOSURE=25ms
    ;;

    (*)
        echo "Usage: $0 <exposure>"
        echo "Available exposures: 5 10 15 20 25"
        exit
    ;;
esac

echo "Setting Exposure $EXPOSUREms..."
. /root/cmv.func
/root/cmv_snap3 -z -e $EXPOSURE

