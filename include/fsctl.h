#ifndef FSCTL_H
#define FSCTL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <uv.h>

// Requests

typedef struct fsctl_lock_s fsctl_lock_t;
typedef struct fsctl_punch_hole_s fsctl_punch_hole_t;
typedef struct fsctl_sparse_s fsctl_sparse_t;
typedef struct fsctl_swap_s fsctl_swap_t;
typedef struct fsctl_swap_at_s fsctl_swap_at_t;

// Callbacks

typedef void (*fsctl_lock_cb)(fsctl_lock_t *req, int status);
typedef void (*fsctl_punch_hole_cb)(fsctl_punch_hole_t *req, int status);
typedef void (*fsctl_sparse_cb)(fsctl_sparse_t *req, int status);
typedef void (*fsctl_swap_cb)(fsctl_swap_t *req, int status);
typedef void (*fsctl_swap_at_cb)(fsctl_swap_at_t *req, int status);

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

struct fsctl_sparse_s {
  uv_work_t req;

  uv_os_fd_t fd;

  fsctl_sparse_cb cb;

  int result;

  void *data;
};

struct fsctl_swap_s {
  uv_work_t req;

  const char *from_path;
  const char *to_path;

  fsctl_swap_cb cb;

  int result;

  void *data;
};

struct fsctl_swap_at_s {
  uv_work_t req;

  uv_os_fd_t from_fd;
  const char *from_path;
  uv_os_fd_t to_fd;
  const char *to_path;

  fsctl_swap_at_cb cb;

  int result;

  void *data;
};

int
fsctl_try_lock (uv_os_fd_t fd, uint64_t offset, size_t length, fsctl_lock_type_t type);

int
fsctl_wait_for_lock (uv_loop_t *loop, fsctl_lock_t *req, uv_os_fd_t fd, uint64_t offset, size_t length, fsctl_lock_type_t type, fsctl_lock_cb cb);

int
fsctl_unlock (uv_os_fd_t fd, uint64_t offset, size_t length);

int
fsctl_punch_hole (uv_loop_t *loop, fsctl_punch_hole_t *req, uv_os_fd_t fd, uint64_t offset, size_t length, fsctl_punch_hole_cb cb);

int
fsctl_sparse (uv_loop_t *loop, fsctl_sparse_t *req, uv_os_fd_t fd, fsctl_sparse_cb cb);

int
fsctl_swap (uv_loop_t *loop, fsctl_swap_t *req, const char *from_path, const char *to_path, fsctl_swap_cb cb);

int
fsctl_swap_at (uv_loop_t *loop, fsctl_swap_at_t *req, uv_os_fd_t from_fd, const char *from_path, uv_os_fd_t to_fd, const char *to_path, fsctl_swap_at_cb cb);

#ifdef __cplusplus
}
#endif
#endif // FSCTL_H
