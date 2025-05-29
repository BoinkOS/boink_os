/*
 * --- Low Level IO (header) --- 
 *
 *
 */

unsigned char pbin(unsigned short port);
void pbout(unsigned short port, unsigned char data);
unsigned short pwin(unsigned short port);
void pwout(unsigned short port, unsigned short data);