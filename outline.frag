#version 400

in vec2 pass_textureCoords;

uniform sampler2D qt_texture;

uniform int qt_opacity;

void main()
{
    float opacity = qt_opacity / 100.0;

    vec4 color = vec4(1.0, 1.0, 1.0, opacity);

    gl_FragColor = color * texture2D(qt_texture, pass_textureCoords);
}
