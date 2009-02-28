# immvertex

## What is it?

immvertex is an implementation of OpenGL's immediate mode function API which runs on top of OpenGLES - specifically, the version that runs on the iPhone.

## Sounds great, how do i use it?

Simply pretend it's still the 90's, and use those GL API's, except instead of "gl" prefix them with "vx".

For example:

  vxBegin(GL_TRIANGLE_STRIP);
    vxColor4f(1.0, 1.0, 0.0, 1.0);
    vxVertex2f(-0.5f, -0.5f);
    vxColor4f(0.0, 1.0, 1.0, 1.0);
    vxVertex2f(0.5f,  -0.5f);
    vxColor4f(0.0, 0.0, 0.0, 0.0);
    vxVertex2f(-0.5f,  0.5f);
    vxColor4f(1.0, 0.0, 1.0, 1.0);
    vxVertex2f(0.5f,   0.5f);
  vxEnd();

Will draw a lovely square to the screen!

A word of warning however. When your app exits, you'd best clear the temporary memory used to build the vertex array!

  vxDestroyState();


## Wait, hasn't this been done before?

Yes - but if you knew that, then why are you looking at this?


Have fun!
