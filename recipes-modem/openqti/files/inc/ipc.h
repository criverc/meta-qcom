/* SPDX-License-Identifier: MIT */

#ifndef _IPC_H_
#define _IPC_H_
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

#define IPC_ROUTER 27     // AF_IB
#define IPC_ROUTER_ADDR 2 // Kernel IPC driver address

// As specified in the kernel
#define IPC_ROUTER_DPM_ADDRTYPE 1
// From the decoded packets, this should be 2, not 1 like for DPM
#define IPC_ROUTER_AT_ADDRTYPE 2

#define IPC_HEXAGON_NODE 0x3
#define IPC_HEXAGON_DPM_PORT 0x1c
#define IPC_HEXAGON_ATFWD_PORT 0x1b

#define SMDCTLPORTNAME "DATA40_CNTL"

#define RMNET_CONN_ID 8
#define IPC_IOCTL_MAGIC 0xc3
#define IOCTL_BIND_TOIPC _IOR(IPC_IOCTL_MAGIC, 4, uint32_t)

#define QTI_IOCTL_MAGIC 'r'
#define GET_LINE_STATE _IOR(QTI_IOCTL_MAGIC, 2, int)

#define IPC_ROUTER_IOCTL_LOOKUP_SERVER                                         \
  _IOC(_IOC_READ | _IOC_WRITE, IPC_IOCTL_MAGIC, 0x2,                           \
       sizeof(struct sockaddr_msm_ipc))
#define EP_LOOKUP _IOR(QTI_IOCTL_MAGIC, 3, struct ep_info)
#define MODEM_OFFLINE _IO(QTI_IOCTL_MAGIC, 4)
#define MODEM_ONLINE _IO(QTI_IOCTL_MAGIC, 5)

#define MAX_PACKET_SIZE 4096 // rmnet max packet size
// IPC Port security rules
#define IOCTL_RULES _IOR(0xC3, 5, struct irsc_rule)
#define IRSC_INSTANCE_ALL 4294967295

// Client release command request
#define CLIENT_REGISTER_REQ 0x22
#define CLIENT_RELEASE_REQ 0x23
#define CLIENT_REG_TIMEOUT 2400000

#define HOST_USES_MODEMMANAGER 0x1a
#define HOST_USES_OFONO 0x02
struct irsc_rule {
  int rl_no;
  uint32_t service;
  uint32_t instance;
  unsigned dummy;
  gid_t group_id[0];
};

// Port names
static const struct {
  uint32_t service;
  uint32_t instance;
  const char *name;
} common_names[] = {
    {0, 0, "Control service"},
    {1, 0, "Wireless Data Service"},
    {2, 0, "Device Management Service"},
    {3, 0, "Network Access Service"},
    {4, 0, "Quality Of Service service"},
    {5, 0, "Wireless Messaging Service"},
    {6, 0, "Position Determination Service"},
    {7, 0, "Authentication service"},
    {8, 0, "AT service"},
    {9, 0, "Voice service"},
    {10, 0, "Card Application Toolkit service (v2)"},
    {11, 0, "User Identity Module service"},
    {12, 0, "Phonebook Management service"},
    {13, 0, "QCHAT service"},
    {14, 0, "Remote file system service"},
    {15, 0, "Test service"},
    {16, 0, "Location service (~ PDS v2)"},
    {17, 0, "Specific absorption rate service"},
    {18, 0, "IMS settings service"},
    {19, 0, "Analog to digital converter driver service"},
    {20, 0, "Core sound driver service"},
    {21, 0, "Modem embedded file system service"},
    {22, 0, "Time service"},
    {23, 0, "Thermal sensors service"},
    {24, 0, "Thermal mitigation device service"},
    {25, 0, "Service access proxy service"},
    {26, 0, "Wireless data administrative service"},
    {27, 0, "TSYNC control service"},
    {28, 0, "Remote file system access service"},
    {29, 0, "Circuit switched videotelephony service"},
    {30, 0, "Qualcomm mobile access point service"},
    {31, 0, "IMS presence service"},
    {32, 0, "IMS videotelephony service"},
    {33, 0, "IMS application service"},
    {34, 0, "Coexistence service"},
    {36, 0, "Persistent device configuration service"},
    {38, 0, "Simultaneous transmit service"},
    {39, 0, "Bearer independent transport service"},
    {40, 0, "IMS RTP service"},
    {41, 0, "RF radiated performance enhancement service"},
    {42, 0, "Data system determination service"},
    {43, 0, "Subsystem control service"},
    {47, 0, "Data Port Mapper service"},
    {49, 0, "IPA control service"},
    {51, 0, "CoreSight remote tracing service"},
    {52, 0, "Dynamic Heap Memory Sharing"},
    {64, 0, "Service registry locator service"},
    {66, 0, "Service registry notification service"},
    {69, 0, "ATH10k WLAN firmware service"},
    {224, 0, "Card Application Toolkit service (v1)"},
    {225, 0, "Remote Management Service"},
    {226, 0, "Open Mobile Alliance device management service"},
    {312, 0, "QBT1000 Ultrasonic Fingerprint Sensor service"},
    {769, 0, "SLIMbus control service"},
    {771, 0, "Peripheral Access Control Manager service"},
    {4096, 0, "TFTP"},
    {4097, 0, "DIAG service"},
};

struct msm_ipc_port_addr {
  uint32_t node_id;
  uint32_t port_id;
};

struct msm_ipc_port_name {
  uint32_t service;
  uint32_t instance;
};

struct msm_ipc_addr {
  unsigned char addrtype;
  union {
    struct msm_ipc_port_addr port_addr;
    struct msm_ipc_port_name port_name;
  } addr;
};
struct sockaddr_msm_ipc {
  unsigned short family;
  struct msm_ipc_addr address;
  unsigned char reserved;
};

enum peripheral_ep_type {
  DATA_EP_TYPE_RESERVED = 0x0,
  DATA_EP_TYPE_HSIC = 0x1,
  DATA_EP_TYPE_HSUSB = 0x2,
  DATA_EP_TYPE_PCIE = 0x3,
  DATA_EP_TYPE_EMBEDDED = 0x4,
  DATA_EP_TYPE_BAM_DMUX = 0x5,
} __attribute__((packed));

struct peripheral_ep_info {
  enum peripheral_ep_type ep_type;
  uint32_t peripheral_iface_id;
} __attribute__((packed));

struct ipa_ep_pair {
  unsigned long int cons_pipe_num;
  unsigned long int prod_pipe_num;
} __attribute__((packed));

struct ep_info {
  struct peripheral_ep_info ph_ep_info;
  struct ipa_ep_pair ipa_ep_pair;
} __attribute__((packed));

struct portmapper_port_map_arr {
  char port_name[11];
  struct peripheral_ep_info epinfo;
};

/* QMI Device handler proto
 *
 *	The idea is to be able to have multiple handlers for
 *  different services subscribed, each with their transaction
 *  IDs to manage or sniff different services.
 *  First one to check is 47 or 0x2F, which is the DPM, to play
 *  with modem init and request DATA40_CNTL properly
 */

struct qmi_device {
  int32_t fd;
  struct sockaddr_msm_ipc socket;
  uint8_t service;
  uint8_t transaction_id;
  uint32_t handle;
  uint8_t cid;
};

struct portmapper_open_request {
  // QMI Header
  uint8_t ctlid;
  uint16_t transaction_id;
  uint16_t msgid;
  uint16_t length;

  // Control list
  uint8_t is_valid_ctl_list;
  uint32_t ctl_list_length;
  struct portmapper_port_map_arr hw_port_map[1];

  // HW list
  uint8_t is_valid_hw_list;
  uint32_t hw_list_length;
  struct ep_info hw_epinfo;

  // SW list
  uint8_t is_valid_sw_list;
  uint32_t sw_list_length;
} __attribute__((packed));

// linux uapi
struct msm_ipc_server_info {
  uint32_t node_id;
  uint32_t port_id;
  uint32_t service;
  uint32_t instance;
};

struct server_lookup_args {
  struct msm_ipc_port_name port_name;
  int num_entries_in_array;
  int num_entries_found;
  uint32_t lookup_mask;
  struct msm_ipc_server_info srv_info[0];
};

int open_ipc_socket(struct qmi_device *qmisock, uint32_t node, uint32_t port,
                    uint32_t service, uint32_t instance,
                    unsigned char address_type);

bool is_server_active(uint32_t node, uint32_t port);

struct msm_ipc_server_info get_node_port(uint32_t service, uint32_t instance);
int find_services();
int init_port_mapper();
int setup_ipc_security();
int get_num_instances_for_service(int service);
void force_close_qmi(int fd);
void send_rmnet_ioctls(int fd);
int track_client_count(uint8_t *pkt, int from, int sz, int fd, int rmnet_fd);

/*

  char qmi_msg_01[] = { 0x00, 0x01, 0x00, 0x21, 0x00, 0x1c, 0x00, 0x10, 0x0d,
  0x00, 0x01, 0x0b, 0x44, 0x41, 0x54, 0x41, 0x34, 0x30, 0x5f, 0x43, 0x4e, 0x54,
  0x4c, 0x11, 0x09, 0x00, 0x01, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00
  };
  */
#endif