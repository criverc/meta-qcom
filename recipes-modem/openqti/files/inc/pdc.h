// SPDX-License-Identifier: MIT
#ifndef __PDC_H__
#define __PDC_H__

#include "openqti.h"
#include "qmi.h"
#include "call.h"
#include <stdbool.h>
#include <stdio.h>
/*
 * Headers for the Persistent Device Configuration Service
 *
 *
 *
 */

enum {
	 PERSISTENT_DEVICE_CONFIG_RESET = 0x0000, 
	 PERSISTENT_DEVICE_CONFIG_GET_SUPPORTED_MSGS = 0x001E, 
	 PERSISTENT_DEVICE_CONFIG_GET_SUPPORTED_FIELDS = 0x001F, 
	 PERSISTENT_DEVICE_CONFIG_INDICATION_REGISTER = 0x0020, 
	 PERSISTENT_DEVICE_CONFIG_CONFIG_CHANGE_IND = 0x0021, 
	 PERSISTENT_DEVICE_CONFIG_GET_SELECTED_CONFIG = 0x0022, 
	 PERSISTENT_DEVICE_CONFIG_SET_SELECTED_CONFIG = 0x0023, 
	 PERSISTENT_DEVICE_CONFIG_LIST_CONFIGS = 0x0024, 
	 PERSISTENT_DEVICE_CONFIG_DELETE_CONFIG = 0x0025, 
	 PERSISTENT_DEVICE_CONFIG_LOAD_CONFIG = 0x0026, 
	 PERSISTENT_DEVICE_CONFIG_ACTIVATE_CONFIG = 0x0027, 
	 PERSISTENT_DEVICE_CONFIG_GET_CONFIG_INFO = 0x0028, 
	 PERSISTENT_DEVICE_CONFIG_GET_CONFIG_LIMITS = 0x0029, 
	 PERSISTENT_DEVICE_CONFIG_GET_DEFAULT_CONFIG_INFO = 0x002A, 
	 PERSISTENT_DEVICE_CONFIG_DEACTIVATE_CONFIG = 0x002B, 
	 PERSISTENT_DEVICE_CONFIG_VALIDATE_CONFIG = 0x002C, 
	 PERSISTENT_DEVICE_CONFIG_GET_FEATURE = 0x002D, 
	 PERSISTENT_DEVICE_CONFIG_SET_FEATURE = 0x002E, 
	 PERSISTENT_DEVICE_CONFIG_REFRESH_IND = 0x002F, 
	 PERSISTENT_DEVICE_CONFIG_GET_CONFIG = 0x0030, 
	 PERSISTENT_DEVICE_CONFIG_NOTIFICATION = 0x0031, 
};

static const struct {
    uint16_t id;
    const char *cmd;
} pdc_svc_commands[] = {
{PERSISTENT_DEVICE_CONFIG_RESET,"PERSISTENT_DEVICE_CONFIG_RESET"}, 
{PERSISTENT_DEVICE_CONFIG_GET_SUPPORTED_MSGS,"PERSISTENT_DEVICE_CONFIG_GET_SUPPORTED_MSGS"}, 
{PERSISTENT_DEVICE_CONFIG_GET_SUPPORTED_FIELDS,"PERSISTENT_DEVICE_CONFIG_GET_SUPPORTED_FIELDS"}, 
{PERSISTENT_DEVICE_CONFIG_INDICATION_REGISTER,"PERSISTENT_DEVICE_CONFIG_INDICATION_REGISTER"}, 
{PERSISTENT_DEVICE_CONFIG_CONFIG_CHANGE_IND,"PERSISTENT_DEVICE_CONFIG_CONFIG_CHANGE_IND"}, 
{PERSISTENT_DEVICE_CONFIG_GET_SELECTED_CONFIG,"PERSISTENT_DEVICE_CONFIG_GET_SELECTED_CONFIG"}, 
{PERSISTENT_DEVICE_CONFIG_SET_SELECTED_CONFIG,"PERSISTENT_DEVICE_CONFIG_SET_SELECTED_CONFIG"}, 
{PERSISTENT_DEVICE_CONFIG_LIST_CONFIGS,"PERSISTENT_DEVICE_CONFIG_LIST_CONFIGS"}, 
{PERSISTENT_DEVICE_CONFIG_DELETE_CONFIG,"PERSISTENT_DEVICE_CONFIG_DELETE_CONFIG"}, 
{PERSISTENT_DEVICE_CONFIG_LOAD_CONFIG,"PERSISTENT_DEVICE_CONFIG_LOAD_CONFIG"}, 
{PERSISTENT_DEVICE_CONFIG_ACTIVATE_CONFIG,"PERSISTENT_DEVICE_CONFIG_ACTIVATE_CONFIG"}, 
{PERSISTENT_DEVICE_CONFIG_GET_CONFIG_INFO,"PERSISTENT_DEVICE_CONFIG_GET_CONFIG_INFO"}, 
{PERSISTENT_DEVICE_CONFIG_GET_CONFIG_LIMITS,"PERSISTENT_DEVICE_CONFIG_GET_CONFIG_LIMITS"}, 
{PERSISTENT_DEVICE_CONFIG_GET_DEFAULT_CONFIG_INFO,"PERSISTENT_DEVICE_CONFIG_GET_DEFAULT_CONFIG_INFO"}, 
{PERSISTENT_DEVICE_CONFIG_DEACTIVATE_CONFIG,"PERSISTENT_DEVICE_CONFIG_DEACTIVATE_CONFIG"}, 
{PERSISTENT_DEVICE_CONFIG_VALIDATE_CONFIG,"PERSISTENT_DEVICE_CONFIG_VALIDATE_CONFIG"}, 
{PERSISTENT_DEVICE_CONFIG_GET_FEATURE,"PERSISTENT_DEVICE_CONFIG_GET_FEATURE"}, 
{PERSISTENT_DEVICE_CONFIG_SET_FEATURE,"PERSISTENT_DEVICE_CONFIG_SET_FEATURE"}, 
{PERSISTENT_DEVICE_CONFIG_REFRESH_IND,"PERSISTENT_DEVICE_CONFIG_REFRESH_IND"}, 
{PERSISTENT_DEVICE_CONFIG_GET_CONFIG,"PERSISTENT_DEVICE_CONFIG_GET_CONFIG"}, 
{PERSISTENT_DEVICE_CONFIG_NOTIFICATION,"PERSISTENT_DEVICE_CONFIG_NOTIFICATION"}, 
};

/* Config types 
 *  MCFG_HW --> PDC_CONFIG_HW
 *  MCFG_SW --> PDC_CONFIG_SW
 *  ????    --> PDC_CONFIG_DB, maybe mcfg pack?
*/
enum {
    PDC_CONFIG_HW = 0x00,
    PDC_CONFIG_SW = 0x01,
    PDC_CONFIG_DB = 0x10,
};

/* Storage */
/* I'm going to need remotefs support, don't I... */
#define PDC_STORAGE_BASEBAND_EFS 0x00
#define PDC_STORAGE_USERSPACE_REMOTE 0x01

/* List settings stuff */
struct pdc_indication_key {
  uint8_t id; // 0x10
  uint16_t len;
  uint32_t key; // I can put whatever I want here? 0xdeadbeef it is
} __attribute__((packed));

struct pdc_setting_type { //PDC_CONFIG_HW
  uint8_t id; // 0x11
  uint16_t len;
  uint32_t data;
} __attribute__((packed));

struct pdc_config_item_header {
    uint32_t config_type; // PDC_CONFIG_*
    uint8_t config_id_size; 
    uint8_t config_id[0];
} __attribute__((packed));

struct pdc_config_list {
    uint8_t id; // 0x11
    uint16_t len; // size of all the config entries
    uint8_t num_elements; // number of config item headers returned
    // struct_pdc_config_item_header[num_elements]
} __attribute__((packed));

const char *get_pdc_command(uint16_t msgid);
void *register_to_pdc_service();
int handle_incoming_pdc_message(uint8_t *buf, size_t buf_len);
#endif
