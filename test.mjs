import test from 'brittle'
import { open } from 'fs/promises'
import { lock, punchHole, setSparse } from './index.js'

test('lock', async t => {
  const file = await open('test/fixture/lock.txt', 'a+')

  await lock(file.fd, { exclusive: true })

  t.pass('lock granted')
})

test('punch hole', async t => {
  const file = await open('test/fixture/sparse.txt', 'w+')

  const empty = 1024 * 1024 * 10 // 10 MB

  const write = Buffer.from('hello world')
  await file.write(write, 0, 11, empty)

  setSparse(file.fd)
  await punchHole(file.fd, 0, empty)

  const read = Buffer.alloc(11)
  await file.read(read, 0, 11, empty)

  t.alike(read, write)
})
