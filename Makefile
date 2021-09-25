all: 
	$(MAKE) -C /lib/modules/$$(uname -r)/build M=$$(pwd)

clean:
	$(MAKE) -C /lib/modules/$$(uname -r)/build M=$$(pwd) clean

.PHONY: clean all