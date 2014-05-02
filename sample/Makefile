#General makefile for Maintaining DS-CUDA SDK
#By Edgar J.

PROJECTS 	?= $(shell find matrixMul/ bandwidth/ broadcast/ claret/ direct/ p2p/ reduction/ vecadd/ -name Makefile)

FILTER-OUT :=

PROJECTS := $(filter-out $(FILTER-OUT), $(PROJECTS))


%.generate:
	+@$(MAKE) -C $(dir $*) $(MAKECMDGOALS)
%.clean:
	+@$(MAKE) -C $(dir $*) clean

all: tcponly ibvonly

tcponly: libdscuda_tcp.a
	$(MAKE) -C vecadd/ vecadd_tcp 
	$(MAKE) -C bandwidth/ bandwidth_tcp 
	$(MAKE) -C broadcast/ broadcast_tcp
	$(MAKE) -C claret/	cras_gpu_tcp
	$(MAKE) -C matrixMul/ matrixMul_tcp
	$(MAKE) -C p2p/ p2p_tcp
	$(MAKE) -C direct/ direct_tcp
	$(MAKE) -C reduction/ reduce_tcp
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
	@echo "TCP DSCUDA samples generated..........."

ibvonly: libdscuda_ibv.a
	$(MAKE) -C vecadd/ vecadd_ibv 
	$(MAKE) -C bandwidth/ bandwidth_ibv 
	$(MAKE) -C broadcast/ broadcast_ibv
	$(MAKE) -C claret/	cras_gpu_ibv
	$(MAKE) -C matrixMul/ matrixMul_ibv
	$(MAKE) -C p2p/ p2p_ibv
	$(MAKE) -C direct/ direct_ibv
	$(MAKE) -C reduction/ reduce_ibv
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
	@echo "IBV DSCUDA samples generated..........."

libdscuda_tcp.a:
	$(MAKE) -C ../src libdscuda_tcp.a

libdscuda_ibv.a:
	$(MAKE) -C ../src libdscuda_ibv.a

tidy:
	@find * | egrep "#" | xargs rm -f
	@find * | egrep "\~" | xargs rm -f

clean: tidy $(addsuffix .clean,$(PROJECTS))

