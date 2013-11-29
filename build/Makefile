SRC_DIR = ../src
GTEST_DIR = ../external/gtest-1.7.0

ifndef ARMA_ROOT
    ARMA_ROOT = /usr/local
endif

VPATH = $(SRC_DIR) \
        $(SRC_DIR)/Domain $(SRC_DIR)/Domain/test \
        $(SRC_DIR)/Mesh $(SRC_DIR)/Mesh/test \
        $(SRC_DIR)/Field $(SRC_DIR)/Field/test \
        $(GTEST_DIR)/include $(GTEST_DIR)/src

INCLUDES = $(foreach path,$(VPATH),-I$(path)) \
           -isystem $(GTEST_DIR)/include -I$(GTEST_DIR) \
           -I$(ARMA_ROOT)/include
LIBRARIES = -L$(ARMA_ROOT)/lib64 -larmadillo -L.

CC = g++
# LC = libtool -static -o
LC = ar cru
CPPFLAGS = -pthread -DUNIT_TEST -g -O0

%.o: %.cpp
	@echo " Building object of $<"
	@$(CC) $(CPPFLAGS) -c $< $(INCLUDES)
%.o: %.cc
	@echo " Building object of $<"
	@$(CC) $(CPPFLAGS) -c $< $(INCLUDES)
%.d: %.cpp %.h
	@echo " Extracting dependencies of $<"
	@$(CC) -MM -MG -MT '$(patsubst %.cpp,%.o,$(notdir $<))' $< -MF $@
%.d: %.cpp
	@echo " Extracting dependencies of $<"
	@$(CC) -MM -MG -MT '$(patsubst %.cpp,%.o,$(notdir $<))' $< -MF $@

all: libgeomtk.a libgtest.a

OBJECTS = $(foreach path,$(shell find ../src -name '*.cpp'), \
		$(patsubst %.cpp,%.o,$(notdir $(path))))
DEPENDS = $(foreach obj,$(OBJECTS),$(patsubst %.o,%.d,$(obj)))

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPENDS)
endif

depends: $(DEPENDS)

libgeomtk.a: $(OBJECTS) $(DEPENDS)
	@echo " Building library $@"
	@$(LC) $@ $(OBJECTS)
libgtest.a: gtest-all.o $(DEPENDS)
	@echo " Building library $@"
	$(LC) $@ $<
test: libgeomtk.a libgtest.a test.o
	@echo " Building test"
	@$(CC) $(CPPFLAGS) -o $@ test.o $(INCLUDES) $(LIBRARIES) -lgeomtk -lgtest
	@echo " Running test"
	@./test

.PHONY:

clean:
	rm -rf *.d *.o libgeomtk.a libgtest.a test
