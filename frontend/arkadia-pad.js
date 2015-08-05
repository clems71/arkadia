'use strict'

let events = require('events')
let HID = require('node-hid')

function findControllerPath(vid, pid) {
  let foundControllerPath = null;
  for (var dev of HID.devices()) {
    if (dev.vendorId == vid && dev.productId == pid) {
      foundControllerPath = dev.path
      break
    }
  }
  return foundControllerPath
}

class ArkadiaPad extends events.EventEmitter {

  constructor () {
    super()

    // Decoding HID packets callbacks (per button)
    this._callbacks = {
      '1': b => (b[0] & 0x01) != 0,
      '2': b => (b[0] & 0x02) != 0,
      '3': b => (b[0] & 0x04) != 0,
      '4': b => (b[0] & 0x08) != 0,
      'up': b => b[2] == 129,
      'down': b => b[2] == 127,
      'left': b => b[1] == 129,
      'right': b => b[1] == 127,
    }
    this._states = new Map()
    this._buffer = new ArrayBuffer(3)

    let self = this
    setInterval(x => self._connect(), 1000)
  }

  _connect() {
    // Already connected ?
    if (this._device != null) return

    // Try to find a peripheral that matches VID/PID couple
    const devPath = findControllerPath(0x04d8, 0x005e)

    // No device, exit and no process
    if (devPath == null) {
      this._device = null
      return
    }

    this._device = new HID.HID(devPath)
    this._states.clear()

    let self = this
    this._device.on('data', x => self._decode(x))
  }

  _decode(data) {
    this._buffer = data

    for (let k in this._callbacks) {
      // Get the latest button state
      let state = this._callbacks[k](this._buffer)
      if (state && !this._states.get(k)) {
        this.emit('pressed', k)
      } else if (!state && this._states.get(k)) {
        this.emit('released', k)
      }
      this._states.set(k, state)
    }
  }

  // This input driver listen for all inputs, no need to register for interrest in some buttons
  listen(name) { }
}

module.exports = ArkadiaPad
