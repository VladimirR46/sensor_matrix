#include <mbed.h>
#include <EthernetInterface.h> // Network interface API
#include "UDPSocket.h"         // Network socket API

#define DIGITAL_OUT 9
#define ANALOG_IN 6

#define _STATIC_IP // enable static IP (dhcp is used instead)
#define _SERVER_PC

#ifdef _STATIC_IP
#define _IP "192.168.0.101"    //"169.254.4.96" //"12.0.3.247"
#define _GATEWAY "192.168.0.1" //"169.254.255.255"//"12.0.0.1"
#define _MASK "255.255.255.0"  //"255.255.0.0"  //"255.255.248.0"
#endif

#ifdef _SERVER_PC
#define _HOST_IP "192.168.0.4" //"169.254.5.149"//"192.168.0.100"
#define _HOST_PORT 4442
#endif

// API objects

EthernetInterface eth;
UDPSocket udp_socket(&eth);
SocketAddress sockAddr(_HOST_IP, _HOST_PORT);

Serial usb(USBTX, USBRX, 115200);

AnalogIn analog_pin[ANALOG_IN] = {PA_3, PC_0, PC_3, PF_3, PF_5, PF_10};
DigitalOut digital_pin[DIGITAL_OUT] = {PF_2, PD_4, PD_3, PD_0, PD_1, PG_0, PD_7, PD_6, PD_5};

unsigned short Matrix[DIGITAL_OUT][ANALOG_IN];

Timer send_timer;

// functions prototypes
void startEth(); // set network,

unsigned int hz = 0;

const uint8_t mess_size = ANALOG_IN * 2 + 1;
unsigned char buff[mess_size];

int main()
{
  startEth();

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

      buff[0] = i;
      uint8_t index = 1;
      for (int j = 0; j < ANALOG_IN; j++)
      {
        // Reads
        uint16_t data = analog_pin[j].read_u16();

        //Pack
        buff[index] = (data & 0xFF00) >> 8;
        buff[index + 1] = data & 0x00FF;

        //Matrix[i][j] = data;
        index += 2;
      }
      digital_pin[i] = 0;

      // Send
      udp_socket.send(buff, mess_size);
    }

    hz++;
    int col = 4;
    if (send_timer.read_ms() >= 1000)
    {
      send_timer.reset();
      usb.printf("hz: %d \n", hz);
      hz = 0;
      //udp_socket.send("y", 1);
      //usb.printf("A0: %d  %d  %d  %d  %d  %d  %d  %d  %d\r\n", Matrix[0][0], Matrix[1][0], Matrix[2][0], Matrix[3][0], Matrix[4][0], Matrix[5][0], Matrix[6][0], Matrix[7][0], Matrix[8][0]);
    }
    //wait(0.1);
  }
}

//-----------------------------------------------------------------------
void startEth()
{

#ifdef _STATIC_IP
  printf("Setting network...\n");
  if (eth.set_network(_IP, _MASK, _GATEWAY))
    printf("Network can't be set\n");
  else
    printf("Network is set\n");
#endif

  printf("Connecting...\n");
  if (eth.connect())
    printf("Connection failure\n");
  else
    printf("Connection succeed\n");

  const char *ip = eth.get_ip_address();
  const char *mac = eth.get_mac_address();
  printf("IP address is: %s\n", ip ? ip : "No IP");
  printf("MAC address is: %s\n", mac ? mac : "No MAC");

  udp_socket.connect(sockAddr);
  udp_socket.set_blocking(false); // set_timeout(0) is equivalent to set_blocking(false) set_timeout(-1) is equivalent to set_blocking(true)
}