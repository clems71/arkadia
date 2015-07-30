precision mediump float;

attribute vec2 position;

uniform vec2 tex0Size;

varying vec2 uv;
varying vec4 xyp_1_2_3;
varying vec4 xyp_5_10_15;
varying vec4 xyp_6_7_8;
varying vec4 xyp_9_14_9;
varying vec4 xyp_11_12_13;
varying vec4 xyp_16_17_18;
varying vec4 xyp_21_22_23;

void main() {
  float x = 1.0 / tex0Size.x;
  float y = 1.0 / tex0Size.y;

  uv = (position.xy * vec2(0.5, -0.5) + vec2(0.5));
  xyp_1_2_3    = uv.xxxy + vec4(      -x, 0.0,   x, -2.0 * y);
  xyp_6_7_8    = uv.xxxy + vec4(      -x, 0.0,   x,       -y);
  xyp_11_12_13 = uv.xxxy + vec4(      -x, 0.0,   x,      0.0);
  xyp_16_17_18 = uv.xxxy + vec4(      -x, 0.0,   x,        y);
  xyp_21_22_23 = uv.xxxy + vec4(      -x, 0.0,   x,  2.0 * y);
  xyp_5_10_15  = uv.xyyy + vec4(-2.0 * x,  -y, 0.0,        y);
  xyp_9_14_9   = uv.xyyy + vec4( 2.0 * x,  -y, 0.0,        y);

  gl_Position = vec4(position.xy, 0.0, 1.0);
}
