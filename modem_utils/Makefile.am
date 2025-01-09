#
#  [2024] SubSeaPulse SRL
#  All Rights Reserved.
#
#  Authors Filippo Campagnaro
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


AM_CXXFLAGS = @SUMDEMO_CXXFLAGS@

lib_LTLIBRARIES = libmodem_utils.la

libmodem_utils_la_SOURCES = chunk.cpp tcp-stream.cpp alsastream.cpp gpio.cpp
libmodem_utils_la_CPPFLAGS = @SUMDEMO_CPPFLAGS@  
libmodem_utils_la_LDFLAGS = @SUMDEMO_LDFLAGS@ 
libmodem_utils_la_LIBADD = @SUMDEMO_LIBADD@ @ALSA_LDADD@ @GPIO_LDADD@
