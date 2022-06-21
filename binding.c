#include <napi-macros.h>
#include <node_api.h>
#include <stdlib.h>
#include <uv.h>

#include "include/fsctl.h"

typedef struct {
  fsctl_lock_t req;

  napi_env env;
  napi_ref ctx;
  napi_ref cb;
} fsctl_napi_lock_t;

typedef struct {
  fsctl_punch_hole_t req;

  napi_env env;
  napi_ref ctx;
  napi_ref cb;
} fsctl_napi_punch_hole_t;

typedef struct {
  fsctl_sparse_t req;

  napi_env env;
  napi_ref ctx;
  napi_ref cb;
} fsctl_napi_sparse_t;

typedef struct {
  fsctl_swap_t req;

  napi_env env;
  napi_ref ctx;
  napi_ref cb;
} fsctl_napi_swap_t;

static void
on_fsctl_lock (fsctl_lock_t *req, int status) {
  fsctl_napi_lock_t *r = (fsctl_napi_lock_t *) req;

  napi_env env = r->env;

  napi_handle_scope scope;
  napi_open_handle_scope(env, &scope);

  napi_value argv[1];

  napi_value ctx;
  napi_get_reference_value(env, r->ctx, &ctx);

  napi_value callback;
  napi_get_reference_value(env, r->cb, &callback);

  napi_create_int32(env, req->result, &argv[0]);

  NAPI_MAKE_CALLBACK(env, NULL, ctx, callback, 1, argv, NULL);

  napi_close_handle_scope(env, scope);

  napi_delete_reference(env, r->ctx);
  napi_delete_reference(env, r->cb);
}

static void
on_fsctl_punch_hole (fsctl_punch_hole_t *req, int status) {
  fsctl_napi_punch_hole_t *r = (fsctl_napi_punch_hole_t *) req;

  napi_env env = r->env;

  napi_handle_scope scope;
  napi_open_handle_scope(env, &scope);

  napi_value argv[1];

  napi_value ctx;
  napi_get_reference_value(env, r->ctx, &ctx);

  napi_value callback;
  napi_get_reference_value(env, r->cb, &callback);

  napi_create_int32(env, req->result, &argv[0]);

  NAPI_MAKE_CALLBACK(env, NULL, ctx, callback, 1, argv, NULL);

  napi_close_handle_scope(env, scope);

  napi_delete_reference(env, r->ctx);
  napi_delete_reference(env, r->cb);
}

static void
on_fsctl_sparse (fsctl_sparse_t *req, int status) {
  fsctl_napi_sparse_t *r = (fsctl_napi_sparse_t *) req;

  napi_env env = r->env;

  napi_handle_scope scope;
  napi_open_handle_scope(env, &scope);

  napi_value argv[1];

  napi_value ctx;
  napi_get_reference_value(env, r->ctx, &ctx);

  napi_value callback;
  napi_get_reference_value(env, r->cb, &callback);

  napi_create_int32(env, req->result, &argv[0]);

  NAPI_MAKE_CALLBACK(env, NULL, ctx, callback, 1, argv, NULL);

  napi_close_handle_scope(env, scope);

  napi_delete_reference(env, r->ctx);
  napi_delete_reference(env, r->cb);
}

static void
on_fsctl_swap (fsctl_swap_t *req, int status) {
  fsctl_napi_swap_t *r = (fsctl_napi_swap_t *) req;

  napi_env env = r->env;

  napi_handle_scope scope;
  napi_open_handle_scope(env, &scope);

  napi_value argv[1];

  napi_value ctx;
  napi_get_reference_value(env, r->ctx, &ctx);

  napi_value callback;
  napi_get_reference_value(env, r->cb, &callback);

  napi_create_int32(env, req->result, &argv[0]);

  NAPI_MAKE_CALLBACK(env, NULL, ctx, callback, 1, argv, NULL);

  napi_close_handle_scope(env, scope);

  napi_delete_reference(env, r->ctx);
  napi_delete_reference(env, r->cb);

  free((char *) req->from_path);
  free((char *) req->to_path);
}

NAPI_METHOD(fsctl_napi_try_lock) {
  NAPI_ARGV(4)
  NAPI_ARGV_UINT32(fd, 0)
  NAPI_ARGV_UINT32(offset, 1)
  NAPI_ARGV_UINT32(len, 2)
  NAPI_ARGV_UINT32(exclusive, 3)

  int err = fsctl_try_lock(
    uv_get_osfhandle(fd),
    offset,
    len,
    exclusive ? FSCTL_WRLOCK : FSCTL_RDLOCK
  );

  NAPI_RETURN_INT32(err);
}

NAPI_METHOD(fsctl_napi_wait_for_lock) {
  NAPI_ARGV(7)
  NAPI_ARGV_BUFFER_CAST(fsctl_napi_lock_t *, req, 0)
  NAPI_ARGV_UINT32(fd, 1)
  NAPI_ARGV_UINT32(offset, 2)
  NAPI_ARGV_UINT32(length, 3)
  NAPI_ARGV_UINT32(exclusive, 4)

  req->env = env;

  napi_create_reference(env, argv[5], 1, &(req->ctx));
  napi_create_reference(env, argv[6], 1, &(req->cb));

  uv_loop_t *loop;
  napi_get_uv_event_loop(env, &loop);

  int err = fsctl_wait_for_lock(
    loop,
    (fsctl_lock_t *) req,
    uv_get_osfhandle(fd),
    offset,
    length,
    exclusive ? FSCTL_WRLOCK : FSCTL_RDLOCK,
    on_fsctl_lock
  );

  NAPI_RETURN_INT32(err);
}

NAPI_METHOD(fsctl_napi_unlock) {
  NAPI_ARGV(3)
  NAPI_ARGV_UINT32(fd, 0)
  NAPI_ARGV_UINT32(offset, 1)
  NAPI_ARGV_UINT32(len, 2)

  int err = fsctl_unlock(uv_get_osfhandle(fd), offset, len);

  NAPI_RETURN_INT32(err);
}

NAPI_METHOD(fsctl_napi_punch_hole) {
  NAPI_ARGV(6)
  NAPI_ARGV_BUFFER_CAST(fsctl_napi_punch_hole_t *, req, 0)
  NAPI_ARGV_UINT32(fd, 1)
  NAPI_ARGV_UINT32(offset, 2)
  NAPI_ARGV_UINT32(length, 3)

  req->env = env;

  napi_create_reference(env, argv[4], 1, &(req->ctx));
  napi_create_reference(env, argv[5], 1, &(req->cb));

  uv_loop_t *loop;
  napi_get_uv_event_loop(env, &loop);

  int err = fsctl_punch_hole(
    loop,
    (fsctl_punch_hole_t *) req,
    uv_get_osfhandle(fd),
    offset,
    length,
    on_fsctl_punch_hole
  );

  NAPI_RETURN_INT32(err);
}

NAPI_METHOD(fsctl_napi_sparse) {
  NAPI_ARGV(4)
  NAPI_ARGV_BUFFER_CAST(fsctl_napi_sparse_t *, req, 0)
  NAPI_ARGV_UINT32(fd, 1)

  req->env = env;

  napi_create_reference(env, argv[2], 1, &(req->ctx));
  napi_create_reference(env, argv[3], 1, &(req->cb));

  uv_loop_t *loop;
  napi_get_uv_event_loop(env, &loop);

  int err = fsctl_sparse(
    loop,
    (fsctl_sparse_t *) req,
    uv_get_osfhandle(fd),
    on_fsctl_sparse
  );

  NAPI_RETURN_INT32(err);
}

NAPI_METHOD(fsctl_napi_swap) {
  NAPI_ARGV(5)
  NAPI_ARGV_BUFFER_CAST(fsctl_napi_swap_t *, req, 0)
  NAPI_ARGV_UTF8_MALLOC(from_path, 1)
  NAPI_ARGV_UTF8_MALLOC(to_path, 2)

  req->env = env;

  napi_create_reference(env, argv[3], 1, &(req->ctx));
  napi_create_reference(env, argv[4], 1, &(req->cb));

  uv_loop_t *loop;
  napi_get_uv_event_loop(env, &loop);

  int err = fsctl_swap(
    loop,
    (fsctl_swap_t *) req,
    from_path,
    to_path,
    on_fsctl_swap
  );

  NAPI_RETURN_INT32(err);
}

NAPI_INIT() {
  NAPI_EXPORT_SIZEOF(fsctl_napi_lock_t)
  NAPI_EXPORT_SIZEOF(fsctl_napi_punch_hole_t)
  NAPI_EXPORT_SIZEOF(fsctl_napi_sparse_t)
  NAPI_EXPORT_SIZEOF(fsctl_napi_swap_t)

  NAPI_EXPORT_FUNCTION(fsctl_napi_try_lock)
  NAPI_EXPORT_FUNCTION(fsctl_napi_wait_for_lock)
  NAPI_EXPORT_FUNCTION(fsctl_napi_unlock)
  NAPI_EXPORT_FUNCTION(fsctl_napi_punch_hole)
  NAPI_EXPORT_FUNCTION(fsctl_napi_sparse)
  NAPI_EXPORT_FUNCTION(fsctl_napi_swap)
}
