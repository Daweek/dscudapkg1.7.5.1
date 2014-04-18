#General File to generate DSCUDA server or client
#By Edgar J.

run_server_ibv:
	./src/server_ibv 

client_tcp_test:
	$(MAKE) -C src/ libdscuda_tcp.a
	$(MAKE) DSCUDA_LIB_TYPE=dscuda_tcp -C sample/vecadd/ vecadd_tcp
	sample/vecadd/vecadd_tcp

client_ibv_test:
	$(MAKE) -C src/ libdscuda_ibv.a
	$(MAKE) DSCUDA_LIB_TYPE=dscuda_ibv -C sample/vecadd/ vecadd_tcp
	sample/vecadd/vecadd_ibv

server_ibv:	
	$(MAKE) -C src/ dscudasvr_ibv
	$(MAKE) -C src/ dscudad_ibv

server_tcp:	
	$(MAKE) -C src/ dscudasvr_tcp
	$(MAKE) -C src/ dscudad_tcp

clean:
	$(MAKE) -C src/ clean
	$(MAKE) -C sample/vecadd clean
