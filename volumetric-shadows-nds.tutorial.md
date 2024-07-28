NDS VOLUMETRIC SHADOWS MINI TUTORIAL
====================================

by R.H.L. (www.robs-basement.de) 2007 - 2008

**IMPORTANT: PLEASE READ THE CONTENTS OF THE README FILE!**

NOTE: I use the term polygon as a synonym for "primitive", which on the NDS are
triangles (-strips) and quads (-strips).

Prepare the hardware by setting the flush mode to manual and w-depth sorting with:

```c
glFlush(GL_TRANS_MANUALSORT | GL_WBUFFERING);
```

Do so in the frame BEFORE the frame you will draw the shadows. Usually, you
would set this up when initializing the application and flush each frame with
those settings.

You might need to sort your translutient polygons / meshes!

Remember, that certain volumetric shadow geometry configurations will cause
artifacts!

These are the three basic steps you need to follow to render volumetric shadows:

1) Render *all* the shadow casting and receiving geometry as usually.

2) For every shadow geometry, draw the "Stencil Mask": Shadow polygons *MUST* be
   translutient, so enable blending and render the shadow geometry's front using
   the following settings:

   ```c
   glPolyFmt(POLY_ALPHA(1) | POLY_CULL_FRONT | POLY_ID(0) | POLY_SHADOW);
   ```

   (Yes, alpha *must* be between 1 and 30 and ID also *MUST* be 0!)

3) The "Stencil Shadow": Now, finally draw the shadow geometry's "back" with
   these settings:

   ```c
   glPolyFmt(POLY_ALPHA(20) | POLY_CULL_BACK | POLY_ID(63) | POLY_SHADOW);
   ```

   (Again, alpha needs to be between 1 and 30, but this time ID *MUST NOT*
   be 0!):

Now you should have a volume shadow on screen. Repeat steps two and three for
all other shadow geometry.

ADDITIONAL NOTES
----------------

- If you're using hardware fogging, you also need to enable it in the second
  shadow poly format or you will get unfogged areas where shadows are (you can
  do that on purpose, e.g. for a flashlight effect).

- Setting different polygon ids for different shadow geometry will result in
  shadows, that accumulate and result in darker regions where they overlap.

- Shadow polygons and vertices count just like normal polygons, so watch the
  hardware's limits!

- Shadows can be coloured - just like real world shadows are rarely black!

- Use closed volumes! The algorithm used by the hardware is called "z-pass".
  Clipping shadow geometry by penetrating it with the "camera", will break it!

- If the shadows are flickering, go back to the beginning and set translutiency
  sorting and depth sorting correctly.

- For an object to shadow itself, simply set the polygon ID for its shadow
  volume to something different than the normal geometry's polygon ID.

- *ALWAYS* follow steps two and three for *EACH* shadow geometry seperately!
  *DO NOT* draw all "masks" first and then all "shadows" - that's a *BAD* idea!
