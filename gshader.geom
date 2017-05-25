#version 400

layout (points) in;
layout (line_strip, max_vertices = 63) out;

// input stuff from Qt
uniform mat4 qt_projectionMatrix;
uniform mat4 qt_modelViewMatrix;
uniform mat4 qt_invertedModelViewMatrix;
uniform mat3 qt_normalMatrix; // transpose of the inverse of the model view matrix

in vec3 data[];

out vec4 color;

#define PI 3.1415
#define SEG 60

// executes for each primitive (point)
void main()
{
    vec4 vertexPos = gl_in[0].gl_Position;

    vertexPos.z = data[0].z;

    gl_Position = qt_projectionMatrix * qt_modelViewMatrix * vertexPos;
    color = vec4(0.0, 0.0, 0.0, 1.0);
    EmitVertex();

    float magnitude = data[0].x;
    float phase = data[0].y;

    vertexPos = gl_in[0].gl_Position + vec4(cos(phase)*magnitude, sin(phase)*magnitude, data[0].z, 0.0);
    gl_Position = qt_projectionMatrix * qt_modelViewMatrix * vertexPos;
    color = vec4(0.0, 0.0, 0.0, 1.0);
    EmitVertex();

    float z = 0.0;
    for(int i=0; i<=SEG; i++) {
        float astep = 2*PI / SEG;

        float angle = astep*i + phase;

        vertexPos = gl_in[0].gl_Position + vec4(cos(angle)*magnitude, sin(angle)*magnitude, data[0].z, 0.0);
        //z+=0.5; // for smooth step

        gl_Position = qt_projectionMatrix * qt_modelViewMatrix * vertexPos;
        if(i%2==0) {
            color = vec4(0.8, 0.8, 0.0, 0.5);
        } else {
            color = vec4(0.0, 0.0, 0.0, 0.5);
        }

        EmitVertex();

    }

    EndPrimitive();
}
