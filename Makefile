#General File to generate DSCUDA server or client
#By Edgar J.

all: client_tcp_test

client_tcp_test:
	$(MAKE) -C src/ libdscuda_tcp.a
	$(MAKE) DSCUDA_LIB_TYPE=tcp -C sample/bandwidth/ bandwidth_tcp
	sample/bandwidth/bandwitdh_tcp

client_ibv_test:
	$(MAKE) -C src/ libdscuda_ibv.a
	$(MAKE) DSCUDA_LIB_TYPE=ibv -C sample/vecadd/ vecadd_tcp
	sample/vecadd/vecadd_ibv

server_ibv:	
	$(MAKE) -C src/ dscudasvr_ibv
	$(MAKE) -C src/ dscudad_ibv

server_tcp:	
	$(MAKE) -C src/ dscudasvr_tcp
	$(MAKE) -C src/ dscudad_tcp

clean:
	$(MAKE) -C sample/ clean
	$(MAKE) -C src/ clean
	