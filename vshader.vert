#version 400

// these are attributes, pointers to buffers
in vec3 qt_vertex;
in vec3 qt_normal;

// input stuff from Qt
uniform mat4 qt_projectionMatrix;
uniform mat4 qt_modelViewMatrix;

// this will contain the color
out vec3 data;

void main()
{
        data = qt_normal; // normals can be interpreted as lines

	vec4 worldPosition = qt_modelViewMatrix * vec4(qt_vertex,1);
        //gl_Position = qt_projectionMatrix * worldPosition;
        gl_Position = vec4(qt_vertex,1);
}
