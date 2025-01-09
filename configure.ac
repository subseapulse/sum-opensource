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

AC_INIT(SUMDEMO,1.0)
AM_INIT_AUTOMAKE 
AC_LANG(C++)
AC_PROG_CXX
AM_PROG_AR
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
LT_INIT
AC_CONFIG_MACRO_DIR([m4])
AC_DISABLE_STATIC
AC_LIBTOOL_WIN32_DLL

#######################################################################

AC_DEFINE(CPP_NAMESPACE,std) 

AC_CHECK_LIB(pthread, pthread_create, [LIBS="$LIBS -lpthread"]) 

SUMDEMO_CPPFLAGS="$SUMDEMO_CPPFLAGS "'-I$(top_srcdir)/janus_wrap'
SUMDEMO_CPPFLAGS="$SUMDEMO_CPPFLAGS "'-I$(top_srcdir)/modem_utils'
SUMDEMO_CPPFLAGS="$SUMDEMO_CPPFLAGS "'-I$(top_srcdir)/janus_executable'
SUMDEMO_CPPFLAGS="$SUMDEMO_CPPFLAGS "'$(GCC6_FLAG)'

SUMDEMO_CXXFLAGS="$SUMDEMO_CXXFLAGS "'-Wall'
SUMDEMO_CXXFLAGS="$SUMDEMO_CXXFLAGS "'-ggdb3'
SUMDEMO_CXXFLAGS="$SUMDEMO_CXXFLAGS "'-std=c++14'

SUMDEMO_LDFLAGS="$SUMDEMO_LDFLAGS "'-L$(top_srcdir)/janus_wrap'
SUMDEMO_LDFLAGS="$SUMDEMO_LDFLAGS "'-L$(top_srcdir)/modem_utils'
SUMDEMO_LDFLAGS="$SUMDEMO_LDFLAGS "'-L/usr/local/share/janus/plugins/'

SUMDEMO_LIBADD="$SUMDEMO_LIBADD -lfftw3f /usr/local/lib/libjanus.a /usr/local/share/janus/plugins/libplugin_016_00.so"

JANUSWRP_LDADD="$JANUSWRP_LDADD "'-ljanus_wrap'
MODEM_UTILS_LDADD="$MODEM_UTILS_LDADD "'-lmodem_utils'
JANUS_LDADD="$JANUS_LDADD "'-lplugin_016_00 -lfftw3f -lfftw3 -ljanus'
ALSA_LDADD="$ALSA_LDADD "'-lasound'
GPIO_LDADD="$GPIO_LDADD "'-lgpiod'

SUMDEMO_LDADD="$SUMDEMO_LDADD $JANUSWRP_LDADD $JANUS_LDADD $ALSA_LDADD $GPIO_LDADD $MODEM_UTILS_LDADD"

AC_SUBST(SUMDEMO_CPPFLAGS) 
AC_SUBST(SUMDEMO_CXXFLAGS)
AC_SUBST(SUMDEMO_LDFLAGS)
AC_SUBST(SUMDEMO_LIBADD)
AC_SUBST(SUMDEMO_LDADD)
AC_SUBST(JANUSWRP_LDADD)
AC_SUBST(MODEM_UTILS_LDADD)
AC_SUBST(JANUS_LDADD)
AC_SUBST(ALSA_LDADD)
AC_SUBST(GPIO_LDADD)

AC_CONFIG_FILES([
  Makefile
  m4/Makefile
  janus_wrap/Makefile
  modem_utils/Makefile
  janus_executable/Makefile
  ])

AC_OUTPUT
