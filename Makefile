#General File to generate DSCUDA server or client
#By Edgar J.

run_server_ibv:
	./src/server_ibv 

server_ibv:	
	$(MAKE) -C src/ dscudasvr_ibv
	$(MAKE) -C src/ dscudad_ibv

server_tcp:	
	$(MAKE) -C src/ dscudasvr_tcp
	$(MAKE) -C src/ dscudad_tcp

clean:
	$(MAKE) -C src/ clean
