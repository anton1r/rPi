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

//Attempt a delay
  sleep(1);
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

  //The default data rate is 800Hz and we don't modify it in this example code

  MMA8452Active();  // Set to active to start reading

}

int main(int argc, char **argv)
{
  printf("MMA8452Q Basic Example\n");

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
