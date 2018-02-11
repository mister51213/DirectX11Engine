# DirectX11Engine

DX11 Engine - Copyright Gregory Weiner 2018

This is a personal side project I am working on; a hand-written 3D engine in C++ for 
Windows Desktop that can be expanded to handle a diversity of real time action games. 
Features realistic 3D graphics using DirectX 11 and physics using the Bullet Physics engine 
(currently disabled).

Technology Notes: 
WASD to move, Mouse to look, Y for wireframe view, U to revert, ESC to exit.
Recommended operating system: Windows 10 x64.

Graphical features include:
Real time dynamic lighting with specular and normal mapping, Dynamic shadowing, Real time Blur, 
Transparency, Refraction effects for transparent materials like glass, ice, and water, 
Reflection (currently disabled), UI for debugging purposes

Coming soon:
Ambient occlusion, Gpu particles, Fully integrated physics

Planned architectural improvements:
-A more encapsulated way of managing textures, render targets, and effects via “materials”
-A more intuitive interface for swapping meshes, textures, and materials
-Performance improvements via model and texture pooling, an instancing pipeline for meshes, and deferred shading
-Shadow refinements using PCF, cascaded shadow maps, and depth bias adjustment
-More realistic reflection via cube mapping
-Upgrade to DirectX 12
