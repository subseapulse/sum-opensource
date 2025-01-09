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

bin_PROGRAMS = janus_executable

janus_executable_SOURCES = janus-executable.cpp

janus_executable_CPPFLAGS = @SUMDEMO_CPPFLAGS@ 
janus_executable_LDFLAGS = @SUMDEMO_LDFLAGS@ 
janus_executable_LDADD = @SUMDEMO_LDADD@ @JANUS_LDADD@ @GPIO_LDADD@ @ALSA_LDADD@ @MODEM_UTILS_LDADD@
