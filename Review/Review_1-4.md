# Review_1-4




## Chapter One - Introduction

1. What's CG used for?
   * Movies: Animation, Special Effects, Performance Capture
   * Computer Games
   * Visualization: Geography, Medical Imaging
   * Training and Simulation
   * Advertising, Design and Art
   * UI Design, CAD-CAM & Design


2. What is Computer Graphics?

   * The science and technology of imaging the world in pixels, such that it provides the real experience -- `looks real, sounds real, feels real` 

   * Computer graphics = synthesis of all visual content

     `modeling + rendering + processing + animation`

     ​

3. What is Interactive Computer Graphics?

   * User controls content, structure, and appearance of objects and their displayed images via rapid visual feedback

   * Basic components of an interactive graphics system : Input, Processing, Display/Output

     ​

4. The Basic Content of Computer Graphics

   * Modeling (shape & appearance): creating and representing the geometry of objects in the 3D world.
   * Rendering (displaying): is a term inherited from art and deals with the creation of 2D shaded images from 3D computer models.


   * Animation (simulating): describing how objects change in time. 



## Chapter Two - Computer Graphics System

1. Current graphics system consists of:
   * An application
   * Graphics library
   * Graphics Hardware



2. RGB FrameBuffer
   * The brightness of each LCD element is controlled by a dedicated memory array called a framebuffer (a buffer that stores the contents of an image pixel by pixel)
   * Each element of the framebuffer is associated with a single pixel on the screen.
   * Typicall, a display allows 256 voltage settings for each of R,G, and B. 
   * We sometimes call each R,G, and B component a channel (so the “red channel” of an image is only the R component per pixel) 



3. The alpha channel

   * In addition to the R, G, and B channels of an image, add a fourth channel called α (transparency/opacity/translucency) 

   * Alpha varies between 0 and 1, Value of 1 represents a completely opaque (不透明的) pixel, while Value of 0 is a completely transparent (透明的) pixel

     ​

4. Physical Imaging System

   * Eye (biology)

   * Camera : film (chemistry), digital (physical + digital)

   * Imaging Principle

     ​

5. What is an image? [什么是图像]

   * We can think of an image as a function f, from $R^2$ to $R$:

     1. `f(x, y)` gives the intensity of a channel at position`(x, y)`

     2. Realistically, we expect the image only to be defined over a rectangle,

        with a finite range:
        $$
        f: [a, b] \times [c, d] \rightarrow [0, 1]
        $$
        ​

     3. A color image is just three functions pasted together. We can write this as a `vector-valued`function: 
        $$
        f(x, y)=\begin{bmatrix} r(x,y) \\ g(x,y) \\ b(x,y) \end{bmatrix}
        $$
        ​

   ​

6. What is a digital image? [什么是数字图像]

   * In computer graphics, we usually operate on digital(discrete) images:
     1. Sample the space on a regular grid
     2. Quantize each sample(round to nearest integer)
     3. If our samples are Δ apart, we can write this as: 

   ​