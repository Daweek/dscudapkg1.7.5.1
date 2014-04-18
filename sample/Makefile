#General makefile for Maintaining DS-CUDA SDK
#By Edgar J.

DSCUDA_LIB_TYPE ?=dscuda_tcp
PROJECTS 	?= $(shell find matrixMul/ bandwidth/ broadcast/ claret/ direct/ p2p/ reduction/ vecadd/ -name Makefile)

FILTER-OUT :=

PROJECTS := $(filter-out $(FILTER-OUT), $(PROJECTS))


#ifneq ($(DSCUDA_LIB_TYPE),dscuda_tcp)
#	DSCUDA_LIB_TYPE=dscuda_ibv
#endif

%.generate:
	+@$(MAKE) -C $(dir $*) $(MAKECMDGOALS)
%.clean:
	+@$(MAKE) -C $(dir $*) clean

all: lib$(DSCUDA_LIB_TYPE).a $(addsuffix .generate, $(PROJECTS)) 
	@echo ".......... "	
	@echo "......... "	
	@echo "........ "	
	@echo "....... "	
	@echo "...... "	
	@echo "..... "	
	@echo ".... "	
	@echo "... "	
	@echo ".. "	
	@echo ". "	
	@echo "DSCUDA samples generated..........."

lib$(DSCUDA_LIB_TYPE).a:
	$(MAKE) -C ../src lib$(DSCUDA_LIB_TYPE).a

tidy:
	@find * | egrep "#" | xargs rm -f
	@find * | egrep "\~" | xargs rm -f

clean: tidy $(addsuffix .clean,$(PROJECTS))

