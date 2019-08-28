#include <mbed.h>

Serial usb(USBTX, USBRX, 115200);

DigitalOut myled(LED1);
AnalogIn ain(PF_10);

int main()
{

  // put your setup code here, to run once:

  while (1)
  {
    usb.printf("A0: %d\r\n", ain.read_u16());
    wait(0.1); // 1 sec
  }
}