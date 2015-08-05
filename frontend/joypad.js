'use strict'

class KeyboardPad {

  constructor () {
    this._buffer = new ArrayBuffer(3)
    const devPath = findControllerPath(0x04d8, 0x005e)
    if (devPath == null) return
    this._device = new HID.HID(devPath)

    var self = this
    this._device.on('data', (data) => {
      self._buffer = data
      if (self.isPressed('left')) {
        console.log('Pressed!!!')
      }
    })

    this._callbacks = {
      '1': b => b[0] & 0x01 != 0,
      '2': b => b[0] & 0x02 != 0,
      'up': b => b[2] == 129,
      'down': b => b[2] == 127,
      'left': b => b[1] == 129,
      'right': b => b[1] == 127,
    }
  }

  isPressed(name) {
    return this._callbacks[name](this._buffer)
  }
}

module.exports = InputSystem
