# SparkEngine
Spark Engine is a 3D engine developed by Marc Guillen and Axel Alavedra during the Game Development grade at CITM-UPC Terrassa.

- Axel's GitHub account: [AxelAlavedra](https://github.com/AxelAlavedra)
- Marc's GitHub account: [Marcgs96](https://github.com/Marcgs96)
- Organization's Github: [ProboxAM](https://github.com/ProboxAM)
- GitHub repository: [SparkEngine](https://github.com/ProboxAM/SparkEngine)

### Features
Engine configuration automatically saves on Engine close.

#### Resource Management
- Upon start, all resources not in library get imported, if they have meta file, it gets imported with meta file info.
- Upon start, all resources already in library get created from meta.
- Resources in library not linked to any meta get deleted on startup.
- On startup if meta file and resource file are not in same folder, it will move meta to resource folder.
- Reference counting.

Not supported yet:
- Delete resources during execution.
- Move resources from folder to folder during execution.
- Create new folders during execution.

#### Project Window
- Drag file outside engine will import it to currently selected folder if file is not in Assets.
- Drag fbx file to Scene Window creates a new Game Object with mesh and texture.
- Drag texture file to a GameObject that has texture in Scene Window will apply the texture.
- Double click a scene file will load it.
- Import settings of selected Resource. (Saving changes reimports resource)
- Double click folder to move to it.
- Click folder name on path "top part of window" will move back to that folder.
- Textures in selected folder get referenced and unreferenced on folder change.
- Slider will change size of file icons.

Alert:
Changing import settings of a Model being used in Scene will make it "invisible" untill Scene is reloaded. This can be done loading Scene again from Project or pressing Play and Stop.

#### Debug
- Component Mesh has a check to show bounding boxes (AABB and OBB).
- Component Texture on Inspector can activate Checkers texture for the GameObject.
- Checkbox in Scene Window to draw Octree.
- Stats button in Game Window shows basic stats like fps, ms and game time.
- Component Camera in inspector has a checker to enable frustum culling.

### Controls
#### Scene
- Mouse Left Click: Select GameObject from Scene.
- Supr: Delete selected GameObject.
- CTRL+S: Save scene.
- CTRL+N: Create new scene.

#### Camera
- Mouse Hold Right:
	- WASD: move forward, left, backward and right
	- Look around
- F: focus camera to selected Game Object
- Hold Alt:
	- Mouse Hold Right Click: look around target
- Mouse Wheel: zoom in/out
- Mouse Hold Shift: move faster


### Work of each Student
#### Axel Alavedra
- Component info on Inspector.
- Quadtree.
- Render scene to texture and show on Scene Window.
- Scene window resizing with aspect ratio.
- Scene serialization. Load/Save scene with UI.
- Save mesh, texture and models to own file format.
- Module resources (meta files, reference counting, etc).
- Engine Start/Pause and state recovery.
- Time module.
- Resource Management.
- Resource Import settings.

#### Marc Guillen
- Component info on Inspector.
- Quadtree.
- Camera Component.
- Frustum culling accelerated by Quadtree.
- Mouse picking accelerated by Quadtree.
- Hierarchy Panel.
- Transform Component.
- ImGuizmo implementation.
- Game Panel that displays the view and projection of the main active camera.
- Module Camera3D.

### Possible innovations

- Octree.
- Game Panel.
- Project window.

### Tools used to develop the engine

- IDE: Microsoft Visual Studio 2017-2019
- External libraries: 
	- SDL 2.0.10
	- OpenGL 4.6.0
	- Glew 2.1.0
	- Assimp 5.0.0
	- DevIL 1.8
	- ImGui 1.74 WIP
	- MathGeoLib 1.5
	- PCG Random Number Generator 0.98
	- JSON for modern C++ 3.7.0
	- PhysFS 3.0.1

## License

MIT License

Copyright (c) 2019 SparkEngine (Axel Alavedra & Marc Guillen)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
