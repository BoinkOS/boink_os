/*
 * --- Low Level IO --- 
 *
 *
 */

// port byte IN
unsigned char pbin(unsigned short port) {	
	// port -> dx, result -> al
	unsigned char result;
	__asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
	return result;
}

// port byte OUT
void pbout(unsigned short port, unsigned char data)  {
	// data -> al, port -> dx
	__asm__("out %%al, %%dx" : :"a" (data), "d" (port));
}

// port word IN
unsigned short pwin(unsigned short port)  {
	// port -> dx, result -> ax
	unsigned short result;
	__asm__("in %%dx, %%ax" : "=a" (result) : "d" (port));
	return result;
}

// port word OUT
void pwout(unsigned short port, unsigned short data)  {
	// data -> ax, port -> dx
	__asm__("out %%ax, %%dx" : :"a" (data), "d" (port));
}