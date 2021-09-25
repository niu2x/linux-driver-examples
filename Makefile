all: memory-driver helloworld

memory-driver: all_drivers
	sudo rmmod memory-device || echo ""
	sudo rm md0 || echo ""
	sudo insmod memory-device/memory-device.ko
	sudo mknod md0 c $$(cat /proc/devices|grep memory-device|cut -d' ' -f1) 0
	sudo chmod 666 md0

helloworld: all_drivers

all_drivers: 
	$(MAKE) -C /lib/modules/$$(uname -r)/build M=$$(pwd)

clean:
	$(MAKE) -C /lib/modules/$$(uname -r)/build M=$$(pwd) clean

.PHONY: clean all_drivers all memory-driver helloworld
