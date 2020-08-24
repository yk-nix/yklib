#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <usb.h>


void print_device(struct usb_device *usb)
{
	if(!usb)
		return;
	struct usb_device *child;
	printf("  name:%s  children:%d  devnum:%d  %p  %p  %p\n", usb->filename, usb->num_children, usb->devnum, usb, usb->next, usb->prev);
	printf("    bLength: %d\n", usb->descriptor.bLength); 
	printf("    bDescriptorType: %d\n", usb->descriptor.bDescriptorType); 
	printf("    bcdUSB: 0x%x\n", usb->descriptor.bcdUSB); 
	printf("    bDeviceClass: %d\n", usb->descriptor.bDeviceClass); 
	printf("    bDeviceSubClass: %d\n", usb->descriptor.bDeviceSubClass); 
	printf("    bDeviceProtocol: %d\n", usb->descriptor.bDeviceProtocol); 
	printf("    bMaxPacketSize0: %d\n", usb->descriptor.bMaxPacketSize0); 
	printf("    idVendor: 0x%x\n", usb->descriptor.idVendor); 
	printf("    idProduct: 0x%x\n", usb->descriptor.idProduct); 
	printf("    bcdDevice: 0x%x\n", usb->descriptor.bcdDevice); 
	printf("    iManufacturer: %d\n", usb->descriptor.iManufacturer); 
	printf("    iProduct: %d\n", usb->descriptor.iProduct); 
	printf("    iSerialNumber: %d\n", usb->descriptor.iSerialNumber); 
	printf("    bNumConfigurations: %d\n", usb->descriptor.bNumConfigurations); 
}

void main(int argc, char *argv[])
{
	struct usb_bus *busses, *bus;
	struct usb_device *dev;
	int ret;
	
	usb_init();
	
	ret = usb_find_busses();
	if(ret < 0) {
		printf("usb_find_busses: %s\n", usb_strerror());
		return;
	}
	printf("buses: %d\n", ret);
	
	ret = usb_find_devices();
	if(ret < 0) {
		printf("usb_find_devices: %s\n", usb_strerror());
		return;
	}
	printf("devices: %d\n", ret);
	
	busses = usb_get_busses();
	if(!busses) {
		printf("usb_get_busses: %s\n", usb_strerror());
		return;
	}

	for(bus = busses; bus; bus = bus->next) {
		printf("%p %s  0x%08d %p %p %p\n", bus, bus->dirname, bus->location, bus->next, bus->prev, bus->root_dev);
		print_device(bus->root_dev);
		for(dev = bus->devices; dev; dev = dev->next) {
			print_device(dev);
		}
	}
}
