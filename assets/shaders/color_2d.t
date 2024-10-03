# Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>

size 32

attrib 0 "a_Position" Vec2f 0
attrib 1 "a_Color" Vec4f 8

gl vert "shaders/glsl150/color.vert"
gl frag "shaders/glsl150/color.frag"
