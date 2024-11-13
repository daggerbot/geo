#version 430

layout(location = 0) uniform mat4 u_Projection;
layout(location = 1) uniform mat4 u_ObjectTransform;

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

out vec4 v_Color;

void main()
{
    gl_Position = u_Projection * u_ObjectTransform * vec4(a_Position, 1.0);
    v_Color = a_Color;
}
