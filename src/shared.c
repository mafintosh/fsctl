#include <stdint.h>
#include <uv.h>

#include "../include/fsctl.h"
#include "platform.h"

int
fsctl_try_lock (uv_os_fd_t fd, uint64_t offset, size_t length, fsctl_lock_type_t type) {
  int err = fsctl__try_lock(fd, offset, length, type);

  if (err == UV_EACCES || err == UV_EAGAIN || err == UV_EBUSY) {
    err = UV_EAGAIN;
  }

  return err;
}

static void
fsctl__lock_work (uv_work_t *req) {
  fsctl_lock_t *r = (fsctl_lock_t *) req->data;

  r->result = fsctl__wait_for_lock(r->fd, r->offset, r->length, r->type);
}

static void
fsctl__lock_after_work (uv_work_t *req, int status) {
  fsctl_lock_t *r = (fsctl_lock_t *) req->data;

  if (r->cb) r->cb(r, r->result);
}

int
fsctl_wait_for_lock (uv_loop_t *loop, fsctl_lock_t *req, uv_os_fd_t fd, uint64_t offset, size_t length, fsctl_lock_type_t type, fsctl_lock_cb cb) {
  req->fd = fd;
  req->offset = offset;
  req->length = length;
  req->type = type;
  req->cb = cb;
  req->req.data = (void *) req;

  return uv_queue_work(loop, &req->req, fsctl__lock_work, fsctl__lock_after_work);
}

int
fsctl_unlock (uv_os_fd_t fd, uint64_t offset, size_t length) {
  return fsctl__unlock(fd, offset, length);
}

static void
fsctl__punch_hole_work (uv_work_t *req) {
  fsctl_punch_hole_t *r = (fsctl_punch_hole_t *) req->data;

  r->result = fsctl__punch_hole(r->fd, r->offset, r->length);
}

static void
fsctl__punch_hole_after_work (uv_work_t *req, int status) {
  fsctl_punch_hole_t *r = (fsctl_punch_hole_t *) req->data;

  if (r->cb) r->cb(r, r->result);
}

int
fsctl_punch_hole (uv_loop_t *loop, fsctl_punch_hole_t *req, uv_os_fd_t fd, uint64_t offset, size_t length, fsctl_punch_hole_cb cb) {
  req->fd = fd;
  req->offset = offset;
  req->length = length;
  req->cb = cb;
  req->req.data = (void *) req;

  return uv_queue_work(loop, &req->req, fsctl__punch_hole_work, fsctl__punch_hole_after_work);
}

static void
fsctl__sparse_work (uv_work_t *req) {
  fsctl_sparse_t *r = (fsctl_sparse_t *) req->data;

  r->result = fsctl__sparse(r->fd);
}

static void
fsctl__sparse_after_work (uv_work_t *req, int status) {
  fsctl_sparse_t *r = (fsctl_sparse_t *) req->data;

  if (r->cb) r->cb(r, r->result);
}

int
fsctl_sparse (uv_loop_t *loop, fsctl_sparse_t *req, uv_os_fd_t fd, fsctl_sparse_cb cb) {
  req->fd = fd;
  req->cb = cb;
  req->req.data = (void *) req;

  return uv_queue_work(loop, &req->req, fsctl__sparse_work, fsctl__sparse_after_work);
}
