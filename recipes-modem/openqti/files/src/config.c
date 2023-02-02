// SPDX-License-Identifier: MIT

#include "../inc/config.h"
#include "../inc/logger.h"
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/reboot.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <syscall.h>
#include <unistd.h>

#define BOOT_FLAG_FILE "/persist/.openqti_boot_done"
struct config_prototype *settings;

int set_persistent_partition_rw() {
  if (system("mount -o remount,rw /persist") < 0) {
    logger(MSG_ERROR, "%s: Error setting partition in RW mode\n", __func__);
    return -1;
  }
  return 0;
}

int set_persistent_partition_ro() {
  if (system("mount -o remount,ro /persist") < 0) {
    logger(MSG_ERROR, "%s: Error setting partition in RO mode\n", __func__);
    return -1;
  }

  return 0;
}

void do_sync_fs() {
  if (system("sync") < 0)
    logger(MSG_ERROR, "%s: Failed to sync fs\n", __func__);
}

int set_initial_config() {
  settings = malloc(sizeof(struct config_prototype));
  settings->custom_alert_tone = 0;
  settings->persistent_logging = 0;
  settings->signal_tracking = 0;
  settings->sms_logging = 0;
  settings->callwait_autohangup = 0;
  settings->automatic_call_recording = 0;
  settings->allow_internal_modem_connectivity = 0;
  settings->first_boot = false;
  snprintf(settings->user_name, MAX_NAME_SZ, "Admin");
  snprintf(settings->modem_name, MAX_NAME_SZ, "Modem");
  return 0;
}

void dump_current_config() {
  logger(MSG_DEBUG,
         "[SETTINGS] Dump current configuration\n"
         "---> Custom alert tone: %i\n"
         "---> Persistent logging: %i\n"
         "---> Signal tracking: %i\n"
         "---> Autokill call waiting: %i\n"
         "---> User name: %s\n"
         "---> Modem name: %s\n",
         settings->custom_alert_tone, settings->persistent_logging,
         settings->signal_tracking, settings->callwait_autohangup,
         settings->user_name, settings->modem_name);
}
int parse_line(char *buf) {
  if (settings == NULL || buf == NULL)
    return 0;

  char setting[64];
  char value[64];
  const char *sep = "=\n"; // get also rid of newlines
  char *token;

  token = strtok(buf, sep);

  strncpy(setting, token, sizeof setting);
  setting[sizeof(setting) - 1] = 0; // making sure that setting is C-String
  token = strtok(NULL, sep);

  if (token == NULL) {
    return 0;
  }

  strncpy(value, token, sizeof value);
  value[sizeof(setting) - 1] = 0;

  if (setting[0] == '#') { // Ignore if it's a comment
    return 1;
  }

  logger(MSG_DEBUG, "%s: Key %s -> val %s -> toint %i\n", __func__, setting,
         value, atoi(value));
  if (strcmp(setting, "custom_alert_tone") == 0) {
    settings->custom_alert_tone = atoi(value);
    return 1;
  }
  if (strcmp(setting, "persistent_logging") == 0) {
    settings->persistent_logging = atoi(value);
    /* As soon as we read this, we remount the partition as rw */
    if (settings->persistent_logging) {
      set_persistent_partition_rw();
    }
    return 1;
  }
  if (strcmp(setting, "signal_tracking") == 0) {
    settings->signal_tracking = atoi(value);
    return 1;
  }

  if (strcmp(setting, "callwait_autohangup") == 0) {
    settings->callwait_autohangup = atoi(value);
    return 1;
  }

  if (strcmp(setting, "sms_logging") == 0) {
    settings->sms_logging = atoi(value);
    return 1;
  }

  if (strcmp(setting, "allow_internal_modem_connectivity") == 0) {
    settings->allow_internal_modem_connectivity = atoi(value);
    return 1;
  }
  
  if (strcmp(setting, "automatic_call_recording") == 0) {
    settings->automatic_call_recording = atoi(value);
    return 1;
  }
  
  if (strcmp(setting, "user_name") == 0) {
    memcpy(settings->user_name, value, strlen(settings->user_name));
    settings->user_name[(sizeof(settings->user_name) - 1)] = 0;
    return 1;
  }
  
  if (strcmp(setting, "modem_name") == 0) {
    strncpy(settings->modem_name, value, sizeof(settings->modem_name));
    settings->modem_name[(sizeof(settings->modem_name) - 1)] = 0;
    return 1;
  }

  // var=val not recognized
  return 0;
}

int write_settings_to_storage() {
  FILE *fp;
  if (set_persistent_partition_rw() < 0) {
    logger(MSG_ERROR, "%s: Can't set persist partition in RW mode\n", __func__);
    return -1;
  }
  logger(MSG_DEBUG, "%s: Open file\n", __func__);
  fp = fopen(CONFIG_FILE_PATH, "w");
  if (fp == NULL) {
    logger(MSG_ERROR, "%s: Can't open config file for writing\n", __func__);
    return -1;
  }
  logger(MSG_INFO, "%s: Store\n", __func__);
  fprintf(fp, "# OpenQTI Config file\n");
  fprintf(fp, "# key=value\n");
  fprintf(fp, "custom_alert_tone=%i\n", settings->custom_alert_tone);
  fprintf(fp, "persistent_logging=%i\n", settings->persistent_logging);
  fprintf(fp, "user_name=%s\n", settings->user_name);
  fprintf(fp, "modem_name=%s\n", settings->modem_name);
  fprintf(fp, "signal_tracking=%i\n", settings->signal_tracking);
  fprintf(fp, "callwait_autohangup=%i\n", settings->callwait_autohangup);
  fprintf(fp, "automatic_call_recording=%i\n", settings->automatic_call_recording);
  fprintf(fp, "sms_logging=%i\n", settings->sms_logging);
  fprintf(fp, "allow_internal_modem_connectivity=%i\n", settings->allow_internal_modem_connectivity);
  logger(MSG_DEBUG, "%s: Close\n", __func__);
  fclose(fp);
  do_sync_fs();
  if (!settings->persistent_logging) {
    if (set_persistent_partition_ro() < 0) {
      logger(MSG_ERROR, "%s: Can't set persist partition in RO mode\n",
             __func__);
      return -1;
    }
  }
  return 0;
}
int write_boot_counter_file(int failed_boots) {
  FILE *fp;
  set_persistent_partition_rw();
  fp = fopen(BOOT_FLAG_FILE, "w");
  if (fp == NULL) {
    logger(MSG_ERROR, "%s: Can't open file for writing!\n", __func__);
    return -1;
  }
  fprintf(fp, "%i", failed_boots);
  fclose(fp);
  return 0;
}

int read_boot_counter_file() {
  FILE *fp;
  int val;
  char buf[4] = {0};
  logger(MSG_DEBUG, "%s: Read boot counter\n", __func__);
  fp = fopen(BOOT_FLAG_FILE, "r");
  if (fp == NULL) {
    logger(MSG_DEBUG, "%s, Creating new boot counter file\n",
            __func__);
    write_boot_counter_file(0);
    return 0;
  }
  if (fgets(buf, 4, fp)) {
    val = atoi(buf);
  }
  fclose(fp);
  logger(MSG_INFO, "%s: Failed boot counter: %i\n", __func__, val);
  if (val < 0 ) {
    val = 0;
  }
  write_boot_counter_file(val+1);
  return val; 
}

int read_settings_from_file() {
  FILE *fp;
  char buf[1024];
  int line = 0;
  bool recreate_cfg_required = false;
  int attempted_boots = 0;
  fp = fopen(CONFIG_FILE_PATH, "r");
  if (fp == NULL) {
    logger(MSG_WARN, "%s: Settings file doesn't exist, creating it\n",
           __func__);
    settings->first_boot = true;
    write_settings_to_storage();
    return 0;
  }

  while (fgets(buf, sizeof buf, fp)) {
    line++;
    if (parse_line(buf) < 1) {
      /* There was some error or unknown in the config file
       * To avoid problems, we regenerate the config file
       * with whatever we could retrieve */
      recreate_cfg_required = true;
    }
  }
  fclose(fp);
  dump_current_config();
  attempted_boots = read_boot_counter_file();
  if (attempted_boots > 3) {
    logger(MSG_WARN, "%s: Enabling persistent logging due to repeated boot failures\n", __func__);
    settings->persistent_logging = 1;
    recreate_cfg_required = true;
  }
  if (recreate_cfg_required) {
    write_settings_to_storage();
  }
  if (!settings->persistent_logging) {
    set_persistent_partition_ro();
  }
  return 0;
}

bool is_first_boot() { return settings->first_boot; }

void clear_ifrst_boot_flag() { settings->first_boot = false; }

int use_persistent_logging() { return settings->persistent_logging; }

char *get_openqti_logfile() {
  if (settings->persistent_logging)
    return PERSISTENT_LOGPATH;

  return VOLATILE_LOGPATH;
}
int use_custom_alert_tone() { return settings->custom_alert_tone; }

int is_signal_tracking_enabled() { return settings->signal_tracking; }

int is_sms_logging_enabled() { return settings->sms_logging; }

int is_internal_connect_enabled() { return settings->allow_internal_modem_connectivity; }

int is_automatic_call_recording_enabled() { return settings->automatic_call_recording; }

int callwait_auto_hangup_operation_mode() {
  return settings->callwait_autohangup;
}

int get_modem_name(char *buff) {
  snprintf(buff, MAX_NAME_SZ, "%s", settings->modem_name);
  return 1;
}

int get_user_name(char *buff) {
  snprintf(buff, MAX_NAME_SZ, "%s", settings->user_name);
  return 1;
}

void set_custom_alert_tone(bool en) {
  if (en) {
    logger(MSG_WARN, "Enabling Custom alert tone\n");
    settings->custom_alert_tone = 1;
  } else {
    logger(MSG_WARN, "Disabling custom alert tone\n");
    settings->custom_alert_tone = 0;
  }
  write_settings_to_storage();
}

void set_automatic_call_recording(uint8_t mode) {
  if (mode == 2) {
    logger(MSG_WARN, "Enabling Automatic Call Recording (record and recycle)\n");
    settings->automatic_call_recording = 2;
  } else if (mode == 1) {
    logger(MSG_WARN, "Enabling Automatic Call Recording\n");
    settings->automatic_call_recording = 1;
  } else {
    logger(MSG_WARN, "Disabling Automatic Call Recording\n");
    settings->automatic_call_recording = 0;
  }
  write_settings_to_storage();
}

void set_sms_logging(bool en) {
  if (en) {
    logger(MSG_WARN, "Enabling SMS logging\n");
    settings->sms_logging = 1;
  } else {
    logger(MSG_WARN, "Disabling SMS logging\n");
    settings->sms_logging = 0;
  }
  write_settings_to_storage();
}

void set_internal_connectivity(bool en) {
  if (en) {
    logger(MSG_WARN, "Enabling Internal networking support (reboot needed)\n");
    settings->allow_internal_modem_connectivity = 1;
  } else {
    logger(MSG_WARN, "Disabling Internal networking support (reboot needed)\n");
    settings->allow_internal_modem_connectivity = 0;
  }
  write_settings_to_storage();
}

void set_persistent_logging(bool en) {
  if (en) {
    logger(MSG_WARN, "Enabling Persistent logs\n");
    if (set_persistent_partition_rw() < 0) {
      logger(MSG_WARN, "Failed to set partition as RW\n");
      settings->persistent_logging = 0;
    } else {
      settings->persistent_logging = 1;
    }
  } else {
    logger(MSG_WARN, "Disabling Persistent logs\n");
    settings->persistent_logging = 0;
  }
  write_settings_to_storage();
}

void set_modem_name(char *name) {
  memset(settings->modem_name, 0, MAX_NAME_SZ);
  snprintf(settings->modem_name, MAX_NAME_SZ, "%s", name);
  write_settings_to_storage();
}

void set_user_name(char *name) {
  memset(settings->user_name, 0, MAX_NAME_SZ);
  snprintf(settings->user_name, MAX_NAME_SZ, "%s", name);
  write_settings_to_storage();
}

void enable_signal_tracking(bool en) {
  if (en) {
    logger(MSG_WARN, "Enabling Signal tracking\n");
    settings->signal_tracking = 1;
  } else {
    logger(MSG_WARN, "Disabling Signal tracking\n");
    settings->signal_tracking = 0;
  }
  write_settings_to_storage();
}

void enable_call_waiting_autohangup(uint8_t en) {
  if (en == 2) {
    logger(MSG_WARN, "Enabling Automatic hang up of calls in waiting state\n");
    settings->callwait_autohangup = 2;
  } else if (en == 1) {
    logger(MSG_WARN, "Enabling Automatic ignore of calls in waiting state\n");
    settings->callwait_autohangup = 1;
  } else {
    logger(MSG_WARN,
           "Disabling Automatic handling of calls in waiting state\n");
    settings->callwait_autohangup = 0;
  }
  write_settings_to_storage();
}
