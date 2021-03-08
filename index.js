const binding = require('node-gyp-build')(__dirname)

module.exports = {
  lock (fd) {
    return binding.fsctl_native_lock(fd) > 0
  },
  unlock (fd) {
    return binding.fsctl_native_unlock(fd) > 0
  },
  sparse (fd) {
    return binding.fsctl_native_sparse(fd) > 0
  },
  punchhole (fd, start, length) {
    const buffer = Buffer.alloc(binding.sizeof_fsctl_native_punchhole_t)
    const ctx = { buffer, resolve: null, reject: null }
    const p = new Promise((resolve, reject) => {
      ctx.resolve = resolve
      ctx.reject = reject
    })

    binding.fsctl_native_punchhole(buffer, ctx, onpunchdone, fd, start, length)
    return p
  }
}

function onpunchdone (res) {
  if (res === 0) this.resolve()
  else this.reject(new Error('Could not punch hole in file'))
}
