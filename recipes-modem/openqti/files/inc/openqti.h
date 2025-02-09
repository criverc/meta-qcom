/* SPDX-License-Identifier: MIT */

#ifndef __OPENQTI_H__
#define __OPENQTI_H__

#include <stdbool.h>
#include <stdint.h>

#define RELEASE_VER "0.8.0"

#define MSG_DEBUG 0
#define MSG_INFO 1
#define MSG_WARN 2
#define MSG_ERROR 3
#define MAX_FD 50

#define LOCKFILE "/tmp/openqti.lock"

#define CALL_STATUS_IDLE 0
#define CALL_STATUS_CS 1
#define CALL_STATUS_VOLTE 2

#define GPIO_DTR "5"        // 0 low, 1 high, input
#define GPIO_WAKEUP_IN "25" // input
#define GPIO_SLEEP_IND "74" // output
#define GPIO_RING_IN "75"   // switch in-out-in?
#define GPIO_MODE_INPUT "in"
#define GPIO_MODE_OUTPUT "out"

#define GPIO_EXPORT_PATH "/sys/class/gpio/export"
#define GPIO_UNEXPORT_PATH "/sys/class/gpio/unexport"
#define GPIO_SYSFS_BASE "/sys/class/gpio/gpio"
#define GPIO_SYSFS_DIRECTION "direction"
#define GPIO_SYSFS_VALUE "value"
#define GPIO_SYSFS_EDGE "edge"

// for handle_pkt
#define FROM_INVALID -1
#define FROM_DSP 0
#define FROM_HOST 1
#define FROM_OPENQTI 2

// For proxy threads
struct node_def {
  int fd;
};
struct node_pair {
  struct node_def node1;
  struct node_def node2;
  bool allow_exit;
};

int start_audio(int type);
int stop_audio();
#endif
