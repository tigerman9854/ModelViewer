#pragma once

static const char* flatVertexShaderSource =
"attribute highp vec4 posAttr;\n"
"attribute lowp vec4 colAttr;\n"
"varying lowp vec4 col;\n"
"uniform highp mat4 matrix;\n"
"void main() {\n"
"   col = colAttr;\n"
"   gl_Position = matrix * posAttr;\n"
"}\n";

static const char* flatFragmentShaderSource =
"varying lowp vec4 col;\n"
"void main() {\n"
"   gl_FragColor = col;\n"
"}\n";

static const GLfloat axes[] = {
    0.f, 0.f, 0.f,
    1.f, 0.f, 0.f,

    0.f, 0.f, 0.f,
    0.f, 1.f, 0.f,

    0.f, 0.f, 0.f,
    0.f, 0.f, 1.f,
};

static const GLfloat axesColors[] = {
    1.f, 0.f, 0.f,
    1.f, 0.f, 0.f,

    0.f, 1.f, 0.f,
    0.f, 1.f, 0.f,

    0.f, 0.f, 1.f,
    0.f, 0.f, 1.f,
};


// Defines a grid from 0 to 10 with 1 spacing.
static const GLfloat grid[] = {
    10.f, 0, 10.f,
    10.f, 0, -10.f,

    9.f, 0, 10.f,
    9.f, 0, -10.f,

    8.f, 0, 10.f,
    8.f, 0, -10.f,

    7.f, 0, 10.f,
    7.f, 0, -10.f,

    6.f, 0, 10.f,
    6.f, 0, -10.f,

    5.f, 0, 10.f,
    5.f, 0, -10.f,

    4.f, 0, 10.f,
    4.f, 0, -10.f,

    3.f, 0, 10.f,
    3.f, 0, -10.f,

    2.f, 0, 10.f,
    2.f, 0, -10.f,

    1.f, 0, 10.f,
    1.f, 0, -10.f,

    0.f, 0, 10.f,
    0.f, 0, -10.f,

    -1.f, 0, 10.f,
    -1.f, 0, -10.f,

    -2.f, 0, 10.f,
    -2.f, 0, -10.f,

    -3.f, 0, 10.f,
    -3.f, 0, -10.f,

    -4.f, 0, 10.f,
    -4.f, 0, -10.f,

    -5.f, 0, 10.f,
    -5.f, 0, -10.f,

    -6.f, 0, 10.f,
    -6.f, 0, -10.f,

    -7.f, 0, 10.f,
    -7.f, 0, -10.f,

    -8.f, 0, 10.f,
    -8.f, 0, -10.f,

    -9.f, 0, 10.f,
    -9.f, 0, -10.f,

    -10.f, 0, 10.f,
    -10.f, 0, -10.f,
};

// Contains enough colors to draw a grid
static const GLfloat gridColors[] = {
    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,

    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,

    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,

    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,

    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,

    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,

    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,

    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,

    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f, 0.6f,
};