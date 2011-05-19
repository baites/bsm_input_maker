CCC      = g++

# Subsystems that have compilable libraries
SUBSYS   = 
LIB      = libbsm_input.so.1.1

# Get list of all heads, sources and objects. Each source (%.cc) whould have
# an object file except programs listed in PROGS
HEADS    = $(wildcard ./interface/*.h)
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

CXXFLAGS = ${DEBUG} -fPIC -pipe -Wall -I../  -I/opt/local/include/ -I${BOOST_ROOT}/include -I${ROOTSYS}/include
LIBS     = -L/usr/lib64 -L/opt/local/lib -lprotobuf -L${BOOST_ROOT}/lib -lboost_system -lboost_filesystem
LDFLAGS  = `root-config --libs` -L/usr/lib64 -L/opt/local/lib -lprotobuf -L${BOOST_ROOT}/lib -lboost_system -lboost_filesystem

# Rules to be always executed: empty ones
.PHONY: all

all: pb obj lib

help:
	@echo "make <rule>"
	@echo
	@echo "Rules"
	@echo "-----"
	@echo
	@echo "  all        compile executables"
	@echo

pb: $(MESSAGES) $(PROTOCOBJS)

obj: $(OBJS)

lib: $(LIB)

prog: $(PROGS)



# Protocol Buffers
#
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
$(LIB): $(OBJS)
	@echo "[+] Generating Library ..."
	$(CCC) -shared -W1,-soname,$(basename $@) $(LDFLAGS) -o $(addprefix ./lib/,$@) $(PROTOCOBJS) $(OBJS)
	@cd ./lib; ln -fs $@ $(basename $@); ln -fs $(basename $@) $(basename $(basename $@))
	@echo



# Executables
#
$(PROGS): $(OBJS) 
	@echo "[+] Compiling programs ..."
	$(CCC) $(CXXFLAGS) `root-config --glibs` $(LIBS) $(OBJS) $(PROTOCOBJS) ./src/$@.cpp -o ./bin/bsm_$@
	@echo



# Cleaning
#
cleanbin:
ifneq ($(strip $(PROGS)),)
	rm -f $(addprefix ./bin/bsm_,$(PROGS))
endif

clean: cleanbin
	rm -f ./obj/*.o
	rm -f ./message/*.pb.{h,cc}
	rm -f ./interface/*.pb.h
	rm -f ./src/*.pb.cc
	rm -f $(addprefix ./lib/,$(basename $(basename $(LIB)))*)
