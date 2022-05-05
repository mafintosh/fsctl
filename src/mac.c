#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <uv.h>

#include "../include/fsctl.h"

static int
fsctl__punch_hole (uv_os_fd_t fd, uint64_t offset, size_t length) {
  struct fpunchhole fpunchhole = {
    .fp_flags = 0,
    .reserved = 0,
    .fp_offset = offset,
    .fp_length = length,
  };

  int res = fcntl(fd, F_PUNCHHOLE, &fpunchhole);
  return res == -1 ? uv_translate_sys_error(errno) : res;
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
