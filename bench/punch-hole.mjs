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
    const { blocks: before } = await file.stat()

    await punchHole(file.fd, 0, n)

    const { blocks: after } = await file.stat()

    b.log(`${before} -> ${after} blocks`)
    b.end()

    await file.close()
  })
}
