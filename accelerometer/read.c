#include <bcm2835.h>
#include <stdio.h>

int main(int argc, char **argv) {
char buf[1];
char whoami;
whoami = 0x0D;
bcm2835_init();
bcm2835_i2c_begin();
bcm2835_i2c_set_baudrate(100000);
bcm2835_i2c_setSlaveAddress(0x1D);
bcm2835_i2c_read_register_rs(&whoami,buf,1);
printf("WHOAMI: %X\n",*buf);

char buff[10];
bcm2835_i2c_read_register_rs(&buf[0], buff,10);
printf("reading 1 : %X\n", *buff);

bcm2835_close();

return 0;
}
