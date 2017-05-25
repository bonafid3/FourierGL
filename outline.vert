#version 400

// these are attributes, pointers to buffers
in vec3 qt_vertex;
in vec2 qt_textureCoords;

// input stuff from Qt
uniform mat4 qt_projectionMatrix;
uniform mat4 qt_modelViewMatrix;

// this will contain the color
//out vec3 color;
out vec2 pass_textureCoords;

void main()
{
    pass_textureCoords = qt_textureCoords;
    vec4 worldPosition = qt_modelViewMatrix * vec4(qt_vertex,1);
    gl_Position = qt_projectionMatrix * worldPosition;
}
