lib_LTLIBRARIES = libmbsimflatxml.la libmbsimxml.la
libmbsimflatxml_la_SOURCES = mbsimflatxml.cc
libmbsimflatxml_la_CPPFLAGS = $(DEPS_CFLAGS) $(PYTHON_CFLAGS) -D_hypot=hypot # -D_hypot is a MSVS/python bug workaround
libmbsimflatxml_la_LIBADD = $(DEPS_LIBS) $(PYTHON_LIBS) -l@BOOST_FILESYSTEM_LIB@ -l@BOOST_SYSTEM_LIB@ $(EXTRA_LIBS)
mbsimflatxmlincludedir = $(includedir)/mbsimxml
mbsimflatxmlinclude_HEADERS = mbsimflatxml.h

libmbsimxml_la_SOURCES = mbsimxml.cc
libmbsimxml_la_CPPFLAGS = $(DEPS_CFLAGS) $(PYTHON_CFLAGS) -D_hypot=hypot # -D_hypot is a MSVS/python bug workaround
libmbsimxml_la_LIBADD = $(DEPS_LIBS) $(PYTHON_LIBS)
mbsimxmlincludedir = $(includedir)/mbsimxml
mbsimxmlinclude_HEADERS = mbsimxml.h


bin_PROGRAMS = mbsimflatxml mbsimxml

mbsimflatxml_CPPFLAGS = $(DEPS_CFLAGS)
mbsimflatxml_LDFLAGS = -L.
mbsimflatxml_LDADD = $(DEPS_LIBS) ./libmbsimflatxml.la -lmbsim -l@BOOST_TIMER_LIB@ -l@BOOST_CHRONO_LIB@
mbsimflatxml_SOURCES = mbsimflatxml-main.cc

mbsimxml_CPPFLAGS = $(DEPS_CFLAGS) $(MBXMLUTILS_CFLAGS)
mbsimxml_LDADD = ./libmbsimflatxml.la ./libmbsimxml.la $(DEPS_LIBS) $(MBXMLUTILS_LIBS) -l@BOOST_FILESYSTEM_LIB@ -l@BOOST_SYSTEM_LIB@
mbsimxml_SOURCES = mbsimxml-main.cc

schemadir = @MBXMLUTILSSCHEMA@/http___www_mbsim-env_de_MBSimXML
dist_schema_DATA = mbsimproject.xsd

mbsimmoduleschemadir = @MBXMLUTILSSCHEMA@/http___www_mbsim-env_de_MBSimModule
dist_mbsimmoduleschema_DATA = mbsimmodule.xsd

# freedesktop.org
freedesktoporgdir = @datadir@/mbsim-env/freedesktop.org
freedesktoporg_DATA = mbsim-env.de.mbsimxml.xml
bin_SCRIPTS = desktopIntegration.sh
