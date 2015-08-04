precision mediump float;

attribute vec2 position;
uniform vec2 scaleXY;

varying vec2 uv;

void main() {
  uv = (position.xy * vec2(0.5, -0.5) + vec2(0.5));
  gl_Position = vec4(position.xy * scaleXY, 0.0, 1.0);
}
