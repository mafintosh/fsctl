import test from 'brittle'
import { open } from 'fs/promises'
import { lock, punchHole } from './index.js'

test('lock', async t => {
  const file = await open('test/fixture/lock.txt', 'a+')

  await lock(file.fd, { exclusive: true })

  t.pass('lock granted')
})

test('punch hole', async t => {
  const file = await open('test/fixture/sparse.txt', 'w+')

  await file.write(Buffer.alloc(1024 * 10, 0x00))

  await punchHole(file.fd)

  t.pass('hole punched')
})
