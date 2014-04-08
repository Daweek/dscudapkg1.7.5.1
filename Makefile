#General File to generate DSCUDA server or client
#By Edgar J.

server:	
	$(MAKE) -C src/ dscudasvr_tcp

clean:
	$(MAKE) -C src/ clean
