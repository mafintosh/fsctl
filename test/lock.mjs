import test from 'brittle'
import { fork } from 'child_process'
import { open } from 'fs/promises'
import { temporaryFile } from 'tempy'
import { tryLock, unlock } from '../index.js'

const isDarwin = process.platform === 'darwin'
const isWindows = process.platform === 'win32'

test('2 exclusive locks, same fd', async (t) => {
  const file = temporaryFile()

  const handle = await open(file, 'w+')
  t.teardown(() => handle.close())

  t.ok(tryLock(handle.fd, { exclusive: true }), 'lock granted')

  if (isWindows) {
    t.absent(tryLock(handle.fd, { exclusive: true }), 'lock denied')
  } else {
    t.ok(tryLock(handle.fd, { exclusive: true }), 'lock granted')
  }
})

test('2 exclusive locks, separate fd', async (t) => {
  const file = temporaryFile()

  const a = await open(file, 'w+')
  t.teardown(() => a.close())

  const b = await open(file, 'w+')
  t.teardown(() => b.close())

  t.ok(tryLock(a.fd, { exclusive: true }), 'lock granted')

  if (isDarwin) {
    t.ok(tryLock(b.fd, { exclusive: true }), 'lock granted')
  } else {
    t.absent(tryLock(b.fd, { exclusive: true }), 'lock denied')
  }
})

test('2 shared locks + 1 exclusive lock, same fd', async (t) => {
  const file = temporaryFile()

  const handle = await open(file, 'w+')
  t.teardown(() => handle.close())

  t.ok(tryLock(handle.fd), 'lock granted')
  t.ok(tryLock(handle.fd), 'lock granted')

  if (isWindows) {
    t.absent(tryLock(handle.fd, { exclusive: true }), 'lock denied')
  } else {
    t.ok(tryLock(handle.fd, { exclusive: true }), 'lock granted')
  }
})

test('2 shared locks + 1 exclusive lock, separate fd', async (t) => {
  const file = temporaryFile()

  const a = await open(file, 'w+')
  t.teardown(() => a.close())

  const b = await open(file, 'w+')
  t.teardown(() => b.close())

  const c = await open(file, 'w+')
  t.teardown(() => c.close())

  t.ok(tryLock(a.fd), 'lock granted')
  t.ok(tryLock(b.fd), 'lock granted')

  if (isDarwin) {
    t.ok(tryLock(c.fd, { exclusive: true }), 'lock granted')
  } else {
    t.absent(tryLock(c.fd, { exclusive: true }), 'lock denied')
  }

  unlock(a.fd)
  unlock(b.fd)

  t.ok(tryLock(c.fd, { exclusive: true }), 'lock granted')
})

test('2 shared locks + 1 exclusive lock, separate process', async (t) => {
  const shared = t.test('grant shared locks')
  shared.plan(2)

  const file = temporaryFile()
  const handle = await open(file, 'w+')

  const p1 = fork('test/fixture/lock.mjs', [file])
  const p2 = fork('test/fixture/lock.mjs', [file])

  p1.on('message', (message) => {
    shared.alike(message, { granted: true }, 'lock granted')
  })

  p2.on('message', (message) => {
    shared.alike(message, { granted: true }, 'lock granted')
  })

  await shared

  const deny = t.test('deny exclusive lock')
  deny.plan(1)

  const p3 = fork('test/fixture/lock.mjs', [file,
    '--mode', 'a+',
    '--exclusive'
  ])

  p3.once('message', (message) => {
    deny.alike(message, { granted: false }, 'lock denied')
  })

  await deny

  const release = t.test('release shared locks')
  release.plan(2)

  p1.on('close', () => release.pass('lock released')).kill()
  p2.on('close', () => release.pass('lock released')).kill()

  await release

  const grant = t.test('grant exclusive lock')
  grant.plan(1)

  p3.on('message', (message) => {
    grant.alike(message, { granted: true }, 'lock granted')
  })

  await grant

  p3.kill()

  await handle.close()
})
