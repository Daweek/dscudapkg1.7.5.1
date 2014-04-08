#General makefile for Maintaining DS-CUDA SDK
#By Edgar J.

PROJECTS 	?= $(shell find vecadd/ -name Makefile)
#PROJECTS 	?= $(shell find matrixMul/ bandwidth/ broadcast/ claret/ direct/ p2p/ reduction/ vecadd/ -name Makefile)

FILTER-OUT :=

PROJECTS := $(filter-out $(FILTER-OUT), $(PROJECTS))

%.generate:
	+@$(MAKE) -C $(dir $*) $(MAKECMDGOALS)
%.clean:
	+@$(MAKE) -C $(dir $*) clean

all: $(addsuffix .generate, $(PROJECTS))
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

build: $(addsuffix .generate, $(PROJECTS))

tidy:
	@find * | egrep "#" | xargs rm -f
	@find * | egrep "\~" | xargs rm -f

clean: tidy $(addsuffix .clean,$(PROJECTS))

