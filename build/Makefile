SRC_ROOT = ../src
VPATH = $(SRC_ROOT) $(SRC_ROOT)/Domain

INCLUDES = $(foreach path,$(VPATH),-I$(path))

CC = g++

%.o: %.cpp
	$(CC) $(CPPFLAGS) -c $< $(INCLUDES)

OBJECTS: Domain.o PlaneDomain.o SphereDomain.o

Domain.o: Domain.h commons.h
PlaneDomain.o: PlaneDomain.h Domain.o
SphereDomain.o: SphereDomain.h Domain.o

all: $(OBJECTS)

.PHONY:

clean:
	rm *.o
