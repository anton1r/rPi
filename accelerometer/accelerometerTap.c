/*
 MMA8452Q Basic Example Code Adapted for C
 Nathan Seidle
 SparkFun Electronics and Tony Rafferty
 November 5, 2012
 */

#include <bcm2835.h> // Used for I2C
#include <stdio.h>
#include <time.h>

// The SparkFun breakout board defaults to 1, set to 0 if SA0 jumper on the bottom of the board is set
char MMA8452_ADDRESS = 0x1D;  // 0x1D if SA0 is high, 0x1C if low

//Define a few of the registers that we will be accessing on the MMA8452
char OUT_X_MSB = 0x01;
char XYZ_DATA_CFG  = 0x0E;
char WHO_AM_I  = 0x0D;
char CTRL_REG1 = 0x2A;

int GSCALE = 2; // Sets full-scale range to +/-2, 4, or 8g. Used to calc real g values.

// Pin definitions
int int1Pin = 2;  // These can be changed, 2 and 3 are the Arduinos ext int pins
int int2Pin = 3;

void sleep(int delay)
{
    double slept;

    time_t start;
    time(&start);
    do{
      time_t now;
      time(&now);

      slept = difftime(now, start);

    }while(slept<delay);
}

// Writes a single byte (dataToWrite) into addressToWrite
void writeRegister(char addressToWrite, char dataToWrite)
{
    char buf[2] = {addressToWrite, dataToWrite};
    bcm2835_i2c_write(buf, 2);

}

// Read bytesToRead sequentially, starting at addressToRead into the dest byte array
void readRegisters(char addressToRead, int bytesToRead, char* dest)
{
    //Set Address to read
    bcm2835_i2c_write(&addressToRead, 1);

    //Get reading
    bcm2835_i2c_read_register_rs(&addressToRead,dest,bytesToRead);

}

// Sets the MMA8452 to standby mode. It must be in standby to change most register settings
void MMA8452Standby()
{
  char buf[2];
  bcm2835_i2c_read_register_rs(&CTRL_REG1,buf,2);
  writeRegister(CTRL_REG1, *buf ^ 0x01); //Clear the active bit to go into standby
}

// Sets the MMA8452 to active mode. Needs to be in this mode to output data
void MMA8452Active()
{
  char buf[2];
  bcm2835_i2c_read_register_rs(&CTRL_REG1,buf,2);

  writeRegister(CTRL_REG1, 0x01); //Set the active bit to begin detection

  char buff[2];
  bcm2835_i2c_read_register_rs(&CTRL_REG1,buff,2);
  if (*buff == 1) // WHO_AM_I should always be 0x2A
  {
    printf("MMA8452Q is Active \n");

  }
  else
  {
    printf("MMA8452Q is Inactive \n");
    while(1) ; // Loop forever if communication doesn't happen
  }
}

void readAccelData(int *destination)
{
  char rawData[6];  // x/y/z accel register data stored here

  readRegisters(OUT_X_MSB, 6, rawData);  // Read the six raw data registers into data array

  // Loop to calculate 12-bit ADC and g value for each axis
  int i;
  for(i=0;i<3;i++)
  {
    int gCount = (rawData[i*2] << 8) | rawData[(i*2)+1];  //Combine the two 8 bit registers into one 12-bit number
    gCount >>= 4; //The registers are left align, here we right align the 12-bit integer

    // If the number is negative, we have to make it so manually (no 12-bit data type)
    if (rawData[i*2] > 0x7F)
    {
      gCount = ~gCount + 1;
      gCount *= -1;  // Transform into negative 2's complement #
    }

    destination[i] = gCount; //Record this gCount into the 3 int array
  }
}

void loop()
{
  char source;


if (digitalRead(int1Pin)==1)  // Interrupt pin, should probably attach to interrupt function
  {

  int accelCount[3];  // Stores the 12-bit signed value
  readAccelData(accelCount);  // Read the x/y/z adc values

  // Now we'll calculate the accleration value into actual g's
  float accelG[3];  // Stores the real accel value in g's
  int i;
  for(i=0;i<3;i++)
  {
    accelG[i] = (float) accelCount[i] / ((1<<12)/(2*GSCALE));  // get actual g value, this depends on scale being set
  }

  // Print out values
  int j;
  for(j=0;j<3;j++)
  {
    //printf(accelG[j], 4);  // Print g values
    printf("%G",accelG[j]);
    printf("\t");  // tabs in between axes
  }
  printf("\n");
}
// If int2 goes high, either p/l has changed or there's been a single/double tap
  if (digitalRead(int2Pin)==1)
  {
    source = readRegister(0x0C);  // Read the interrupt source reg.
    if ((source & 0x10)==0x10)  // If the p/l bit is set, go check those registers
      portraitLandscapeHandler();
    else if ((source & 0x08)==0x08)  // Otherwise, if tap register is set go check that
      tapHandler();
  }

//Attempt a delay
  sleep(1);
}

void tapHandler()
{
  char source = readRegister(0x22);  // Reads the PULSE_SRC register

  if ((source & 0x10)==0x10)  // If AxX bit is set
  {
    if ((source & 0x08)==0x08)  // If DPE (double puls) bit is set
      printf("    Double Tap (2) on X");  // tabbing here for visibility
    else
      printf("Single (1) tap on X");

    if ((source & 0x01)==0x01)  // If PoIX is set
      printf(" +\n");
    else
      printf(" -\n");
  }
  if ((source & 0x20)==0x20)  // If AxY bit is set
  {
    if ((source & 0x08)==0x08)  // If DPE (double puls) bit is set
      printf("    Double Tap (2) on Y");
    else
      printf("Single (1) tap on Y");

    if ((source & 0x02)==0x02)  // If PoIY is set
      printf(" +\n");
    else
      printf(" -\n");
  }
  if ((source & 0x40)==0x40)  // If AxZ bit is set
  {
    if ((source & 0x08)==0x08)  // If DPE (double puls) bit is set
      printf("    Double Tap (2) on Z");
    else
      printf("Single (1) tap on Z");
    if ((source & 0x04)==0x04)  // If PoIZ is set
      printf(" +\n");
    else
      printf(" -\n");
  }
}

// This function will read the p/l source register and
// print what direction the sensor is now facing
void portraitLandscapeHandler()
{
  char pl = readRegister(0x10);  // Reads the PL_STATUS register
  switch((pl&0x06)>>1)  // Check on the LAPO[1:0] bits
  {
  case 0:
    printf("Portrait up, ");
    break;
  case 1:
    printf("Portrait Down, ");
    break;
  case 2:
    printf("Landscape Right, ");
    break;
  case 3:
    printf("Landscape Left, ");
    break;
  }
  if (pl&0x01)  // Check the BAFRO bit
    printf("Back");
  else
    printf("Front");
  if (pl&0x40)  // Check the LO bit
    printf(", Z-tilt!");
  printf("\n");
}

// Initialize the MMA8452 registers
// See the many application notes for more info on setting all of these registers:
// http://www.freescale.com/webapp/sps/site/prod_summary.jsp?code=MMA8452Q
void initMMA8452()
{
    char buf[1];
    bcm2835_i2c_read_register_rs(&WHO_AM_I,buf,1);

  if (*buf == CTRL_REG1) // WHO_AM_I should always be 0x2A
  {
    printf("MMA8452Q is online...\n");

  }
  else
  {
    printf("WHOAMI not as expected: %X\n",*buf);
    while(1) ; // Loop forever if communication doesn't happen
  }

  MMA8452Standby();  // Must be in standby to change registers

  // Set up the full scale range to 2, 4, or 8g.
  char fsr = GSCALE;
  if(fsr > 8) fsr = 8; //Easy error check
  fsr >>= 2; // Neat trick, see page 22. 00 = 2G, 01 = 4A, 10 = 8G
  writeRegister(XYZ_DATA_CFG, fsr);

  // Setup the 3 data rate bits, from 0 to 7
  writeRegister(0x2A, readRegister(0x2A) & ~(0x38));
  if (dataRate <= 7)
    writeRegister(0x2A, readRegister(0x2A) | (dataRate << 3));

// Set up portrait/landscap registers - 4 steps:
  // 1. Enable P/L
  // 2. Set the back/front angle trigger points (z-lock)
  // 3. Set the threshold/hysteresis angle
  // 4. Set the debouce rate
  // For more info check out this app note: http://cache.freescale.com/files/sensors/doc/app_note/AN4068.pdf
  writeRegister(0x11, 0x40);  // 1. Enable P/L
  writeRegister(0x13, 0x44);  // 2. 29deg z-lock (don't think this register is actually writable)
  writeRegister(0x14, 0x84);  // 3. 45deg thresh, 14deg hyst (don't think this register is writable either)
  writeRegister(0x12, 0x50);  // 4. debounce counter at 100ms (at 800 hz)

  /* Set up single and double tap - 5 steps:
   1. Set up single and/or double tap detection on each axis individually.
   2. Set the threshold - minimum required acceleration to cause a tap.
   3. Set the time limit - the maximum time that a tap can be above the threshold
   4. Set the pulse latency - the minimum required time between one pulse and the next
   5. Set the second pulse window - maximum allowed time between end of latency and start of second pulse
   for more info check out this app note: http://cache.freescale.com/files/sensors/doc/app_note/AN4072.pdf */
  writeRegister(0x21, 0x7F);  // 1. enable single/double taps on all axes
  // writeRegister(0x21, 0x55);  // 1. single taps only on all axes
  // writeRegister(0x21, 0x6A);  // 1. double taps only on all axes
  writeRegister(0x23, 0x20);  // 2. x thresh at 2g, multiply the value by 0.0625g/LSB to get the threshold
  writeRegister(0x24, 0x20);  // 2. y thresh at 2g, multiply the value by 0.0625g/LSB to get the threshold
  writeRegister(0x25, 0x08);  // 2. z thresh at .5g, multiply the value by 0.0625g/LSB to get the threshold
  writeRegister(0x26, 0x30);  // 3. 30ms time limit at 800Hz odr, this is very dependent on data rate, see the app note
  writeRegister(0x27, 0xA0);  // 4. 200ms (at 800Hz odr) between taps min, this also depends on the data rate
  writeRegister(0x28, 0xFF);  // 5. 318ms (max value) between taps max

  // Set up interrupt 1 and 2
  writeRegister(0x2C, 0x02);  // Active high, push-pull interrupts
  writeRegister(0x2D, 0x19);  // DRDY, P/L and tap ints enabled
  writeRegister(0x2E, 0x01);  // DRDY on INT1, P/L and taps on INT2

  //The default data rate is 800Hz and we don't modify it in this example code
  MMA8452Active();  // Set to active to start reading

}

int main(int argc, char **argv)
{
  printf("MMA8452Q Basic Example\n");

  // Set up the interrupt pins, they're set as active high, push-pull
  //pinMode(int1Pin, INPUT);
  //digitalWrite(int1Pin, LOW);
  //pinMode(int2Pin, INPUT);
  //digitalWrite(int2Pin, LOW);

  bcm2835_init();
  bcm2835_i2c_begin();
  bcm2835_i2c_set_baudrate(100000);
  bcm2835_i2c_setSlaveAddress(MMA8452_ADDRESS);

  initMMA8452(); //Test and intialize the MMA8452

while(1){
    loop();
  }
  return 0;
}
