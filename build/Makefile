SRC_DIR = ../src
GTEST_DIR = ../external/gtest-1.7.0

ifndef ARMA_ROOT
    ARMA_ROOT = /usr/local
endif

VPATH = $(SRC_DIR) \
        $(SRC_DIR)/Domain $(SRC_DIR)/Domain/test \
        $(SRC_DIR)/Mesh $(SRC_DIR)/Mesh/test \
        $(SRC_DIR)/Field $(SRC_DIR)/Field/test \
        $(GTEST_DIR)/src

INCLUDES = $(foreach path,$(VPATH),-I$(path)) \
           -isystem $(GTEST_DIR)/include -I$(GTEST_DIR) \
           -I$(ARMA_ROOT)/include
LIBRARIES = -L$(ARMA_ROOT)/lib64 -larmadillo -L.

CC = g++
#LC = libtool --mode=link $(CC) -static -o
LC = ar cru
CPPFLAGS = -pthread

%.o: %.cpp
	$(CC) $(CPPFLAGS) -c $< $(INCLUDES)
%.o: %.cc
	$(CC) $(CPPFLAGS) -c $< $(INCLUDES)

all: libgeomtk.a libgtest.a

OBJECTS = Domain.o PlaneDomain.o SphereDomain.o \
          Mesh.o StructuredMesh.o RLLMesh.o \
          Field.o RLLScalarField.o RLLVectorField.o

Domain.o: Domain.h commons.h
PlaneDomain.o: PlaneDomain.h Domain.o
SphereDomain.o: SphereDomain.h Domain.o
Mesh.o: Mesh.h commons.h
StructuredMesh.o: StructuredMesh.h Mesh.o
RLLMesh.o: RLLMesh.h StructuredMesh.o
Field.o: Field.h Mesh.o
RLLScalarField.o: RLLScalarField.h Field.o RLLMesh.o
RLLVectorField.o: RLLVectorField.h Field.o RLLMesh.o
test.o: Domain_test.h SphereDomain_test.h \
        StructuredMesh_test.h RLLScalarField_test.h

libgeomtk.a: $(OBJECTS)
	$(LC) $@ $^
libgtest.a: gtest-all.o
	$(LC) $@ $^
test: libgeomtk.a libgtest.a test.o
	$(CC) $(CPPFLAGS) -o $@ test.o $(INCLUDES) $(LIBRARIES) -lgeomtk -lgtest
	./test

.PHONY:

clean:
	rm -rf *.o libgeomtk.a libgtest.a test
