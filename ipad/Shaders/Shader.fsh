//
//  Shader.fsh
//  Milkblocks
//
//  Created by Robert Manson on 10/28/10.
//  Copyright 2010 Archetype Inc. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
