// Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>

#version 150 core

uniform mat4 u_Projection;
uniform mat4 u_ObjectTransform;

in vec3 a_Position;
in vec4 a_Color;

out vec4 v_Color;

void main()
{
    gl_Position = u_Projection * u_ObjectTransform * vec4(a_Position, 1);
    v_Color = a_Color;
}
