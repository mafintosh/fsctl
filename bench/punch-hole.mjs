import bench from 'nanobench'
import { open } from 'fs/promises'
import prettyBytes from 'pretty-bytes'
import { punchHole } from '../index.js'

for (let i = 12; i < 31; i++) {
  const n = 2 ** i

  bench(`punching ${prettyBytes(n, { binary: true })}`, async function (b) {
    const file = await open(`bench/data/sparse-${n}.txt`, 'w+')
    await file.write(Buffer.alloc(n, 0xff))

    b.start()
    await count('before')

    await punchHole(file.fd, 0, n)

    await count('after')
    b.end()

    await file.close()

    async function count (msg) {
      const { blocks } = await file.stat()
      b.log(`blocks ${msg}: ${blocks}`)
    }
  })
}
