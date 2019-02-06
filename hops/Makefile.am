## Process this file with automake to produce Makefile.in

# These are good warnings to turn on by default
if GCC
AM_CXXFLAGS = -Wall -Wwrite-strings -Woverloaded-virtual -Wno-sign-compare
endif


# Unittests run by 'make check':

TESTS = coordinates_test coordinates2d_test coordinates3d_test \
	polygons_test quaternion_test \
	aperture_test heliostat_actuation_test heliostat_test


# A rule to build field_layout.pb.{h,cc} using protoc on field_layout.proto:

src/field_layout.pb.h src/field_layout.pb.cc: src/field_layout.proto
	$(PROTOC) --proto_path=$(srcdir) --cpp_out=$(srcdir) $<


# The executable:

bin_PROGRAMS = hops

hops_SOURCES = \
  math/coordinates.h \
  math/coordinates2d.cc \
  math/coordinates3d.cc \
  math/quaternion.cc \
  math/polygons.cc \
  solpos/solpos.cc \
  src/field_layout.pb.cc \
  src/aperture.cc \
  src/field.cc \
  src/heliostat.cc \
  src/heliostat_actuation.cc \
  src/io.cc \
  src/optic.cc \
  src/optic_container.cc \
  src/optic_container_connection.cc \
  src/optical_simulation_main.cc \
  src/receiver.cc \
  src/receiver_element.cc \
  src/receiver_surfaces.cc \
  src/simulation.cc \
  src/sun.cc \
  src/sun_calculators.cc

hops_CXXFLAGS = -DNDEBUG  # disable DCHECK in the binary
hops_LDADD = -lgflags -lglog -lprotobuf


# The unittests:

coordinates_test_SOURCES = \
  math/coordinates_test.cc \
  gtest/src/gtest-all.cc \
  gtest/src/gtest_main.cc

coordinates_test_CXXFLAGS = -I$(top_srcdir)/gtest/include -I$(top_srcdir)/gtest
coordinates_test_LDADD = -lgflags -lglog -lprotobuf

coordinates2d_test_SOURCES = \
  math/coordinates2d_test.cc \
  gtest/src/gtest-all.cc \
  gtest/src/gtest_main.cc \
  math/coordinates2d.cc

coordinates2d_test_CXXFLAGS = -I$(top_srcdir)/gtest/include -I$(top_srcdir)/gtest
coordinates2d_test_LDADD = -lgflags -lglog -lprotobuf

coordinates3d_test_SOURCES = \
  math/coordinates3d_test.cc \
  gtest/src/gtest-all.cc \
  gtest/src/gtest_main.cc \
  math/coordinates3d.cc \
  math/quaternion.cc

coordinates3d_test_CXXFLAGS = -I$(top_srcdir)/gtest/include -I$(top_srcdir)/gtest
coordinates3d_test_LDADD = -lgflags -lglog -lprotobuf

polygons_test_SOURCES = \
  math/polygons_test.cc \
  gtest/src/gtest-all.cc \
  gtest/src/gtest_main.cc \
  math/coordinates2d.cc \
  math/polygons.cc

polygons_test_CXXFLAGS = -I$(top_srcdir)/gtest/include -I$(top_srcdir)/gtest
polygons_test_LDADD = -lgflags -lglog -lprotobuf

quaternion_test_SOURCES = \
  math/quaternion_test.cc \
  gtest/src/gtest-all.cc \
  gtest/src/gtest_main.cc \
  math/quaternion.cc

quaternion_test_CXXFLAGS = -I$(top_srcdir)/gtest/include -I$(top_srcdir)/gtest
quaternion_test_LDADD = -lgflags -lglog -lprotobuf

aperture_test_SOURCES = \
  src/aperture_test.cc \
  gtest/src/gtest-all.cc \
  gtest/src/gtest_main.cc \
  src/aperture.cc \
  src/optic.cc \
  math/coordinates.h \
  math/coordinates2d.cc \
  math/coordinates3d.cc \
  math/quaternion.cc \
  math/polygons.cc

aperture_test_CXXFLAGS = -I$(top_srcdir)/gtest/include -I$(top_srcdir)/gtest
aperture_test_LDADD = -lgflags -lglog -lprotobuf

heliostat_actuation_test_SOURCES = \
  src/heliostat_actuation_test.cc \
  gtest/src/gtest-all.cc \
  gtest/src/gtest_main.cc \
  src/field_layout.pb.cc \
  src/heliostat_actuation.cc \
  math/coordinates.h \
  math/coordinates2d.cc \
  math/coordinates3d.cc \
  math/quaternion.cc

heliostat_actuation_test_CXXFLAGS = -I$(top_srcdir)/gtest/include -I$(top_srcdir)/gtest
heliostat_actuation_test_LDADD = -lgflags -lglog -lprotobuf

heliostat_test_SOURCES = \
  src/heliostat_test.cc \
  gtest/src/gtest-all.cc \
  gtest/src/gtest_main.cc \
  src/heliostat.cc \
  src/heliostat_actuation.cc \
  src/optic.cc \
  math/coordinates.h \
  math/coordinates2d.cc \
  math/coordinates3d.cc \
  math/quaternion.cc \
  math/polygons.cc

heliostat_test_CXXFLAGS = -I$(top_srcdir)/gtest/include -I$(top_srcdir)/gtest
heliostat_test_LDADD = -lgflags -lglog -lprotobuf

noinst_PROGRAMS = $(TESTS)
