const fs = require('fs')
const ctl = require('./')

const size = 500 * 1024 * 1024
const big = Buffer.alloc(size)

fs.writeFileSync('big', big)

const fd = fs.openSync('big', 'r+')

console.log(fd, sparseness(fd))

console.time()
const res = ctl.punchhole(fd, 0, Math.floor(size / 4 / 1024) * 4096)
console.timeEnd()

// console.log('res', res)

res.then(function () {
  console.log('holepunched!')
  console.log(fd, sparseness(fd))
})

function sparseness (fd) {
  const st = fs.fstatSync(fd)
  return (st.blocks * 512) / st.size
}
