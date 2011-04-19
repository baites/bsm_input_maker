CCC      = g++

# Subsystems that have compilable libraries
SUBSYS   = 
LIB      = libPBInput.so

# Get list of all heads, sources and objects. Each source (%.cc) whould have
# an object file except programs listed in PROGS
HEADS    = $(wildcard ./interface/*.h)
MSGS     = $(wildcard ./proto/*LinkDef.h)
DICS     = $(wildcard ./src/*LinkDef.h)
SRCS     = $(filter-out %.pb.cc,$(wildcard ./src/*.cc))

OBJS       = $(foreach obj,$(addprefix ./obj/,$(patsubst %.cc,%.o,$(notdir $(SRCS)))),$(obj))

PROTOCS    = $(wildcard ./proto/*.proto)
MESSAGES   = $(subst ./proto/,./message/,$(patsubst %.proto,%.pb.h,$(PROTOCS)))
PROTOCOBJS = $(subst ./proto/,./obj/,$(patsubst %.proto,%.o,$(PROTOCS)))

# List of programs with main functions to be filtered out of objects
PROGS    = $(patsubst ./src/%.cpp,%,$(wildcard ./src/*.cpp))

# Flags used in compilation
ifeq ($(strip $(DEBUG)),)
	DEBUG = -O2
else
	DEBUG = -O0 -g
endif

CXXFLAGS = ${DEBUG} -pipe -Wall -I./  -I/opt/local/include/ -I${BOOST_ROOT}/include -I${ROOTSYS}/include
LIBS     = -L/opt/local/lib -lprotobuf -L${BOOST_ROOT}/lib -lboost_filesystem -lboost_system
LDFLAGS  = -shared -W1 `root-config --libs` -L/opt/local/lib -lprotobuf

# Rules to be always executed: empty ones
.PHONY: all

all: pb $(OBJS) lib $(PROGS)

help:
	@echo "make <rule>"
	@echo
	@echo "Rules"
	@echo "-----"
	@echo
	@echo "  all        compile executables"
	@echo



# Protocol Buffers
#
pb: $(MESSAGES) $(PROTOCOBJS)

$(MESSAGES):
	@echo "[+] Generating Protocol Buffers ..."
	protoc -I=proto --cpp_out message $(patsubst message/%.pb.h,proto/%.proto,$@)
	@pushd ./interface &> /dev/null; ln -s ../$@; popd &> /dev/null
	@pushd src &> /dev/null; ln -s ../$(patsubst %.h,%.cc,$@); popd &> /dev/null
	@echo

$(PROTOCOBJS): $(MESSAGES)
	@echo "[+] Compiling Protocol Buffers ..."
	$(CCC) $(CXXFLAGS) -I./message -c $(addprefix ./message/,$(patsubst %.o,%.pb.cc,$(notdir $@))) -o $@
	@echo



# Regular compilcation
#
$(OBJS): $(SRCS) $(HEADS)
	@echo "[+] Compiling objects ..."
	$(CCC) $(CXXFLAGS) -c $(addprefix ./src/,$(patsubst %.o,%.cc,$(notdir $@))) -o $@
	@echo



# Libraries
#
lib: $(LIB)

$(LIB): $(OBJS)
	@echo "[+] Generating Library ..."
	$(CCC) $(LDFLAGS) -o $(addprefix ./lib/,$@) $(PROTOCOBJS)
	@echo



# Executables
#
$(PROGS): $(OBJS) 
	@echo "[+] Compiling programs ..."
	$(CCC) $(CXXFLAGS) `root-config --glibs` $(LIBS) $(OBJS) $(PROTOCOBJS) ./src/$@.cpp -o ./bin/$@
	@echo



# Cleaning
#
cleanbin:
ifneq ($(strip $(PROGS)),)
	rm -f $(addprefix ./bin/,$(PROGS))
endif

clean: cleanbin
	rm -f ./obj/*.o
	rm -f ./message/*.pb.{h,cc}
	rm -f ./interface/*.pb.h
	rm -f ./src/*.pb.cc
	rm -f $(addprefix ./lib/,$(LIB))
