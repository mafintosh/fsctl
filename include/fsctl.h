#ifndef FSCTL_H
#define FSCTL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <uv.h>

// Requests

typedef struct fsctl_lock_s fsctl_lock_t;
typedef struct fsctl_punch_hole_s fsctl_punch_hole_t;

// Callbacks

typedef void (*fsctl_lock_cb)(fsctl_lock_t *req, int status);
typedef void (*fsctl_punch_hole_cb)(fsctl_punch_hole_t *req, int status);

typedef enum {
  FSCTL_RDLOCK = 1,
  FSCTL_WRLOCK,
} fsctl_lock_type_t;

struct fsctl_lock_s {
  uv_work_t req;

  uv_os_fd_t fd;
  uint64_t offset;
  uint64_t length;
  fsctl_lock_type_t type;

  fsctl_lock_cb cb;

  int result;

  void *data;
};

struct fsctl_punch_hole_s {
  uv_work_t req;

  uv_os_fd_t fd;
  uint64_t offset;
  uint64_t length;

  fsctl_punch_hole_cb cb;

  int result;

  void *data;
};

int
fsctl_lock (uv_loop_t *loop, fsctl_lock_t *req, uv_os_fd_t fd, uint64_t offset, size_t length, fsctl_lock_type_t type, fsctl_lock_cb cb);

int
fsctl_try_lock (uv_os_fd_t fd, uint64_t offset, size_t length, fsctl_lock_type_t type);

int
fsctl_unlock (uv_os_fd_t fd, uint64_t offset, size_t length);

int
fsctl_punch_hole (uv_loop_t *loop, fsctl_punch_hole_t *req, uv_os_fd_t fd, uint64_t offset, size_t length, fsctl_punch_hole_cb cb);

int
fsctl_set_sparse (uv_os_fd_t fd);

#ifdef __cplusplus
}
#endif
#endif // FSCTL_H
