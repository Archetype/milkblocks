//
//  Shader.vsh
//  Milkblocks
//
//  Created by Robert Manson on 10/28/10.
//  Copyright 2010 Archetype Inc. All rights reserved.
//

attribute vec4 position;
attribute vec4 color;

varying vec4 colorVarying;

uniform float translate;

void main()
{
    gl_Position = position;
    gl_Position.y += sin(translate) / 2.0;

    colorVarying = color;
}
