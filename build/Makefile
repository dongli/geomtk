SRC_ROOT = ../src
VPATH = $(SRC_ROOT) $(SRC_ROOT)/Domain $(SRC_ROOT)/Mesh

INCLUDES = $(foreach path,$(VPATH),-I$(path))

CC = g++

%.o: %.cpp
	$(CC) $(CPPFLAGS) -c $< $(INCLUDES)

OBJECTS: Domain.o PlaneDomain.o SphereDomain.o \
         Mesh.o StructuredMesh.o

Domain.o: Domain.h commons.h
PlaneDomain.o: PlaneDomain.h Domain.o
SphereDomain.o: SphereDomain.h Domain.o
Mesh.o: Mesh.h commons.h
StructuredMesh.o: StructuredMesh.h Mesh.o

all: $(OBJECTS)

.PHONY:

clean:
	rm *.o
