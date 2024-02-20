#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <linux/joystick.h>
#include <string.h>
#include <arpa/inet.h>
#include "msp.h"
#include <stdbool.h>

#define _BSD_SOURCE
#define NUM_AXES 12
#define NUM_BUTTONS 12

#define AXIS_THROTTLE 1
#define AXIS_YAW 0
#define AXIS_ROLL 3 
#define AXIS_PITCH 4

#define BUTTON_ARM 0 

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 5762

int axes[NUM_AXES] = {0};
int buttons[NUM_BUTTONS] = {0};

int sockfd;

uint8_t mspCRC(uint8_t checksum, uint8_t *data, uint8_t length)
{
  while (length-- > 0)
  {
    checksum ^= *data++;
  }

  return (checksum);
}

uint32_t mspSendData(uint8_t size, uint8_t command, uint8_t *data)
{
  uint8_t msp_send_buffer[256] = {0};

  msp_send_buffer[0] = '$';
  msp_send_buffer[1] = 'M';
  msp_send_buffer[2] = '<';
  msp_send_buffer[3] = size;
  msp_send_buffer[4] = command;
  for (uint32_t i = 0; i < size; i++)
  {
    msp_send_buffer[i+5] = *(data+i);
  }

  uint8_t crc = 0;
  crc = mspCRC(0, (uint8_t *)(msp_send_buffer+3), size+3);
  msp_send_buffer[size+5] = crc;

  // Send data to server
  if (send(sockfd, msp_send_buffer, size+6, 0) < 0) {
    perror("Send failed");
    exit(EXIT_FAILURE);
  }

  return (size+6);
}

void initTCPSocket(void)
{
  struct sockaddr_in server_addr;

  // Create a TCP socket
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }

  // Initialize server address
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(SERVER_PORT);

  // Convert IPv4 and IPv6 addresses from text to binary form
  if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
    perror("Invalid address/ Address not supported");
    exit(EXIT_FAILURE);
  }

  // Connect to server
  if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("Connection failed");
    exit(EXIT_FAILURE);
  }
}

uint16_t joystickToRcValue(int32_t input)
{
  float zero2one = (float)input / 32767.0f;
  int16_t rc_value = 1500 + (int16_t)(zero2one*500);

  return ((uint16_t)rc_value);
}

uint16_t buttonToRCValue(int32_t button, bool state)
{
  static bool button_state[NUM_BUTTONS] = {0};
  static bool last_button_state[NUM_BUTTONS] = {0}; 
  uint16_t output = 0;

  // if (button_state[button] == last_button_state[button])
  // {
  //   last_button_state[button] = state;
  // }

  if (state == 1)
  {
    if (last_button_state[button] != state)
    {
      button_state[button] = !button_state[button];
    }
  }

  output = 1200 + (uint16_t)button_state[button]*600;

  last_button_state[button] = state;
  return (output);
}

int main() 
{
  struct msp_set_raw_rc_t raw_rc = {0};
  // static unsigned long long next_print_at = 0;
  const char *device = "/dev/input/js0"; // Path to the joystick device
  int fd = open(device, O_RDONLY);

  initTCPSocket();
  
  if (fd == -1) 
  {
    perror("Error opening joystick device");
    return 1;
  }
  
  printf("Joystick device opened successfully\n");
  
  struct js_event e;
  
  while (1) 
  {
    ssize_t bytes = read(fd, &e, sizeof(e));
      
    if (bytes == -1) 
    {
      perror("Error reading joystick events");
      break;
    }
      
    if (bytes == sizeof(e)) 
    {
      switch (e.type & ~JS_EVENT_INIT) {
        case JS_EVENT_AXIS:      
          if (e.number < NUM_AXES)
          {
            axes[e.number] = e.value;
          }
          // printf("Axis %u: %d\n", e.number, e.value);
          break;
        case JS_EVENT_BUTTON:
          if (e.number < NUM_BUTTONS)
          {
            buttons[e.number] = e.value;
          }
          // printf("Button %u: %d\n", e.number, e.value);
          break;
        }
    }


    printf("\033[2J");

    for (int axis = 0; axis < NUM_AXES; axis++)
    {
      printf("Axis %d: %d\n", axis, axes[axis]);
    }

    for (int axis = 0; axis < NUM_AXES; axis++)
    {
      printf("Button %d: %d\n", axis, buttons[axis]);
    }
    
    raw_rc.channels[0] =  joystickToRcValue(axes[AXIS_ROLL]);
    raw_rc.channels[1] =  joystickToRcValue(-axes[AXIS_PITCH]); 
    raw_rc.channels[2] =  joystickToRcValue(-axes[AXIS_THROTTLE]); 
    raw_rc.channels[3] =  joystickToRcValue(axes[AXIS_YAW]); 
    raw_rc.channels[4] =  buttonToRCValue(BUTTON_ARM, buttons[BUTTON_ARM]);

    for (uint32_t i = 0; i < 5; i++)
    {
      printf("Channel %d: %d\n", i,  raw_rc.channels[i]);
    }

    mspSendData(sizeof(raw_rc), MSP_SET_RAW_RC, (uint8_t *)&raw_rc);

    sleep(0.05);
  }
  
  close(fd);
  
  return 0;
}
