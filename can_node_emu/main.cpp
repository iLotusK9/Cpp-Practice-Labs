#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <sstream>


class CanInterface {
 public:
  virtual void initialize(std::string_view can_if_name) = 0;
  virtual int sent(int id, );
};

int main() {
  // Create a socket for CAN communication
  int socket_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
  if (socket_fd < 0) {
    std::cerr << "Error opening socket: " << strerror(errno) << std::endl;
    return -1;
  }

  // Define the CAN interface (e.g., vcan0)
  struct ifreq ifr;
  std::strcpy(ifr.ifr_name, "vcan0");

  // Get the index of the CAN interface
  if (ioctl(socket_fd, SIOCGIFINDEX, &ifr) < 0) {
    std::cerr << "Error getting interface index: " << strerror(errno)
              << std::endl;
    return -1;
  }

  struct sockaddr_can addr;
  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;

  // Bind the socket to the CAN interface
  if (bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    std::cerr << "Error binding socket: " << strerror(errno) << std::endl;
    return -1;
  }

  // Prepare the CAN frame to send
  struct can_frame frame;
  frame.can_id = 0x123;  // CAN ID
  frame.can_dlc = 8;     // Data length
  for (int i = 0; i < frame.can_dlc; ++i) {
    frame.data[i] = i;
  }

  // Send the CAN frame
  if (send(socket_fd, &frame, sizeof(struct can_frame), 0) < 0) {
    std::cerr << "Error sending CAN frame: " << strerror(errno) << std::endl;
    return -1;
  }

  std::cout << "CAN frame sent successfully!" << std::endl;

  // Close the socket
  close(socket_fd);
  return 0;
}
