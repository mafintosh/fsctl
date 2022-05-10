import bench from 'nanobench'
import { open } from 'fs/promises'
import prettyBytes from 'pretty-bytes'
import { punchHole } from '../index.js'

for (let i = 12; i < 31; i++) {
  const n = 2 ** i

  bench(`punching ${prettyBytes(n)}`, async function (b) {
    const file = await open(`bench/data/sparse-${n}.txt`, 'w+')

    await file.write(Buffer.alloc(n))

    b.start()
    await punchHole(file.fd, 0, n)
    b.end()

    await file.close()
  })
}
