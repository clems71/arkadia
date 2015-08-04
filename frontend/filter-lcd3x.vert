precision mediump float;

attribute vec2 position;

uniform vec2 tex0Size;
uniform vec2 scaleXY;

varying vec2 uv;
varying vec2 omega;

void main() {
  uv = (position.xy * vec2(0.5, -0.5) + vec2(0.5));
  omega = 3.141592654 * 2.0 * tex0Size;
  gl_Position = vec4(position.xy * scaleXY, 0.0, 1.0);
}
