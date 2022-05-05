#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <uv.h>

#include "../include/fsctl.h"

static int
fsctl__lock (uv_os_fd_t fd, uint64_t offset, size_t length, fsctl_lock_type_t type, bool block) {
  struct flock flock = {
    .l_start = offset,
    .l_len = length,
    .l_pid = 0,
    .l_type = type == FSCTL_WRLOCK ? F_WRLCK : F_RDLCK,
    .l_whence = SEEK_SET,
  };

  int res = fcntl(fd, block ? F_SETLKW : F_SETLK, &flock);
  return res == -1 ? uv_translate_sys_error(errno) : res;
}

static void
fsctl__lock_work (uv_work_t *req) {
  fsctl_lock_t *r = (fsctl_lock_t *) req->data;

  r->result = fsctl__lock(r->fd, r->offset, r->length, r->type, true);
}

static void
fsctl__lock_after_work (uv_work_t *req, int status) {
  fsctl_lock_t *r = (fsctl_lock_t *) req->data;

  if (r->cb) r->cb(r, r->result);
}

int
fsctl_lock (uv_loop_t *loop, fsctl_lock_t *req, uv_os_fd_t fd, uint64_t offset, size_t length, fsctl_lock_type_t type, fsctl_lock_cb cb) {
  req->fd = fd;
  req->offset = offset;
  req->length = length;
  req->type = type;
  req->cb = cb;
  req->req.data = (void *) req;

  return uv_queue_work(loop, &req->req, fsctl__lock_work, fsctl__lock_after_work);
}

int
fsctl_try_lock (uv_os_fd_t fd, uint64_t offset, size_t length, fsctl_lock_type_t type) {
  int err = fsctl__lock(fd, offset, length, type, false);

  if (err == UV_EACCES || err == UV_EAGAIN) err = UV_EAGAIN;

  return err;
}

int
fsctl_unlock (uv_os_fd_t fd, uint64_t offset, size_t length) {
  struct flock flock = {
    .l_start = offset,
    .l_len = length,
    .l_pid = 0,
    .l_type = F_UNLCK,
    .l_whence = SEEK_SET,
  };

  int res = fcntl(fd, F_SETLK, &flock);
  return res == -1 ? uv_translate_sys_error(errno) : res;
}
