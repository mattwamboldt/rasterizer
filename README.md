# README #

### Description ###

The Rasterizer is a learning project, where I'm attempting to build a software 3d renderer.
It currently supports some basic vector shapes using variants on the besenham algorithm, so no anti aliasing.
It also will render a basic model in OBJ format using gouraud shading. All rendering is done via software to an
SDL Surface which is then rendered to the window.

### Instructions ###

The code can be built using Visual Studio 2013 or later using the solution found in the 
source folder. Alternatively you can download the executable from the downloads section.

On boot you should see a vector clock in the upper left, and a shaded spinning monkey in the middle.
Suzanne the monkey should be familiar to anyone who has used Blender, a free 3d modelling program.