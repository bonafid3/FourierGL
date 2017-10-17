#version 400

in vec2 pass_textureCoords;

uniform sampler2D qt_texture;

uniform int qt_opacity;

out vec4 fragColor;

void main()
{
    float opacity = qt_opacity / 100.0;

    vec4 color = vec4(1.0, 1.0, 1.0, opacity);

    fragColor = color * texture(qt_texture, pass_textureCoords);
}
