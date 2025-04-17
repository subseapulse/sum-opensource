#!/usr/bin/env bash
#
#  [2024] SubSeaPulse SRL
#  All Rights Reserved.
#  Author Filippo Campagnaro
#  
# This is free software: you can redistribute it and/or modify it        
# under the terms of the GNU General Public License version 3 as         
# published by the Free Software Foundation.                             
#                                                                        
# This program is distributed in the hope that it will be useful, but    
# WITHOUT ANY WARRANTY; without even the implied warranty of FITNESS     
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for       
# more details.                                                          
#                                                                        
# You should have received a copy of the GNU General Public License      
# along with this program. If not, see <http://www.gnu.org/licenses/>.   
#

set -e

printHelp() {
cat << EOF

OPTIONS:
   -h    Show this message
   -p    Prefix Path
   -c    clean repository

   e.g. $0 -p <your_home> -t 1
EOF
}
CLEAN=0
PREFIX=""

while getopts “hp:c” OPTION
do
    case $OPTION in
        h)
            printHelp
            exit 0
            ;;
        p)
            PREFIX=$OPTARG
            ;;
        c)
            CLEAN=1
            ;;
        ?)
            printHelp
            exit 0
            ;;
    esac
done

if [[ $CLEAN -eq 0 && -z $PREFIX ]];
then
    printHelp
    exit 0
fi

if [ $CLEAN -eq 1 ];
then
  make distclean
  exit 0
fi

touch ~/.profile
if ! grep -q '/usr/local/share/janus/plugins' ~/.profile; then
    echo 'Adding Janus plugin path /usr/local/share/janus/plugins to ~/.profile'
    echo "export LD_LIBRARY_PATH="/usr/local/share/janus/plugins${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}"" | sudo tee -a ~/.profile >/dev/null
fi

./autogen.sh
./configure --prefix $PREFIX

make
make install
