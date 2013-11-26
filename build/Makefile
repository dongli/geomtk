SRC_DIR = ../src
GTEST_DIR = ../external/gtest-1.7.0

VPATH = $(SRC_DIR) \
        $(SRC_DIR)/Domain $(SRC_DIR)/Domain/test \
        $(SRC_DIR)/Mesh $(SRC_DIR)/Mesh/test \
        $(GTEST_DIR)/src

INCLUDES = $(foreach path,$(VPATH),-I$(path)) \
           -isystem $(GTEST_DIR)/include -I$(GTEST_DIR)

CC = g++
LIBTOOL = libtool
CPPFLAGS = 

%.o: %.cpp
	$(CC) $(CPPFLAGS) -c $< $(INCLUDES)
%.o: %.cc
	$(CC) $(CPPFLAGS) -c $< $(INCLUDES)

all: libgeomtk.a libgtest.a

OBJECTS = Domain.o PlaneDomain.o SphereDomain.o \
          Mesh.o StructuredMesh.o

Domain.o: Domain.h commons.h
PlaneDomain.o: PlaneDomain.h Domain.o
SphereDomain.o: SphereDomain.h Domain.o
Mesh.o: Mesh.h commons.h
StructuredMesh.o: StructuredMesh.h Mesh.o
test.o: Domain_test.h SphereDomain_test.h StructuredMesh_test.h

libgeomtk.a: $(OBJECTS)
	$(LIBTOOL) -static -o $@ $^
libgtest.a: gtest-all.o
	$(LIBTOOL) -static -o $@ $^
test: libgeomtk.a libgtest.a test.o
	$(CC) $(CPPFLAGS) -o $@ $^ $(INCLUDES)
	./test

.PHONY:

clean:
	rm -rf *.o libgeomtk.a libgtest.a test
