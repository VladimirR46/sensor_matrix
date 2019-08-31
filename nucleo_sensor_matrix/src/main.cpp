#include <mbed.h>

#define DIGITAL_OUT 9
#define ANALOG_IN 6

Serial usb(USBTX, USBRX, 115200);

DigitalOut myled(PH_1);

AnalogIn analog_pin[ANALOG_IN] = {PA_3, PC_0, PC_3, PF_3, PF_5, PF_10};
DigitalOut digital_pin[DIGITAL_OUT] = {PF_2, PH_1, PH_0, PD_0, PD_1, PG_0, PD_7, PD_6, PD_5};

unsigned short Matrix[DIGITAL_OUT][ANALOG_IN];

Timer send_timer;

int main()
{
  // Init
  for (int i = 0; i < DIGITAL_OUT; i++)
    digital_pin[i] = 0;

  send_timer.start();
  // main loop
  while (1)
  {
    for (int i = 0; i < DIGITAL_OUT; i++)
    {
      digital_pin[i] = 1;
      wait_us(100);
      for (int j = 0; j < ANALOG_IN; j++)
      {
        // Reads
        Matrix[i][j] = analog_pin[j].read_u16();
      }
      digital_pin[i] = 0;
    }

    int col = 7;
    if (send_timer.read_ms() >= 100)
    {
      send_timer.reset();
      usb.printf("A0: %d  %d  %d  %d  %d  %d  \r\n", Matrix[col][0], Matrix[col][1], Matrix[col][2], Matrix[col][3], Matrix[col][4], Matrix[col][5]);
    }
    //wait(0.1);
  }
}