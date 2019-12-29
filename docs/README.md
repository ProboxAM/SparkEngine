# SparkEngine
Spark Engine is a 3D engine developed by Marc Guillen and Axel Alavedra during the Game Development grade at CITM-UPC Terrassa. The entire development has been supervised by the lecturer Ricard Pillosu. 

The code is written in C++.

## Authors

![Axel](images/axel_image.jpeg)

- Axel's GitHub account: [AxelAlavedra](https://github.com/AxelAlavedra)

### Contribution

* Component info on Inspector.
* Quadtree.
* Render scene to texture and show on Scene Window.
* Scene window resizing with aspect ratio.
* Scene serialization. Load/Save scene with UI.
* Save mesh, texture and models to own file format.
* Module resources (meta files, reference counting, etc).
* Engine Start/Pause and state recovery.
* Time module.
* Resource Management.
* Resource Import settings.
* Skinning.
* Bones import and Skeleton attachment.
* Animation import.
* Animation import settings ( Creation of clips ).
* Animator Controller own file. Import/Save/Load of that file.

![Marc](images/marc_image.jpg)

- Marc's GitHub account: [Marcgs96](https://github.com/Marcgs96)

### Contribution

* Component info on Inspector.
* Quadtree.
* Camera Component.
* Frustum culling accelerated by Quadtree.
* Mouse picking accelerated by Quadtree.
* GameObject hierarchy system.
* Transform Component.
* ImGuizmo implementation.
* Game Panel that displays the view and projection of the main active camera.
* Module Camera3D.
* Animation time management and interpolation between keys.
* Blending between animation.
* Animator Controller state machine.
* Animator editor.

## Main core sub-systems

* It has a hierarchy of gameObjects, each can have multiple components that define and complement them (transformation, mesh, material, animation, bone, camera).
* Can manipulate their transformation directly from the scene panel through the usage of a guizmo.
* Can freely add, remove and change parent and order of gameObjects into the hierarchy panel.
* It has a file and resource systems, to manage asset files, imort them and store them as binaries for easily usage.
* It is using a QuadTree to optimize frustum culling of static gameObjects.
* It has scene serialization of json files through rapidjson library.n any game engine an easy-to-use UI is a must and, thanks to the ImGui library, this was super fast an simple to implement. The way we did it was having a GUI module that works as a manager for all ImGui panels. To approach the Inspector panel, we implemented an OnEditor method at each component so, when a game object is selected, we draw its components on the Inspector.

## Skeletal animation sub-system

### Features

* It allows the user to import fbx files that contain animations using Assimp.
* The user can create clips of the imported animation and set the clip on loop.
* He can also create an asset called Animation Controller. A state machine that allows the user to create different states and transitions between them. He can create/modify the name/select a clip resource/Delete a state, and Create/Set the trigger/Set the blend value/Delete a transition.

### System in-depth

* It reads animations and bones from a fbx file, then we link the bones to a mesh so we can modify it through skinning.
* Then, when a animator component is attached to a game object and a Animator Controller to the component, the object bones will be updated from the animation key values in each frame when we hit play.
* On update, every key frame for every bone, an update transform is called which interpolates the current key values with the next key values of every transform component (position, rotation, scale).
* At the end of the animation, if the clip is setted to loop it will start again.
* If theres a transition to another animation and the transition is triggered, both animation transforms will blend for smoother results.

* Editor gif
![eg](https://github.com/ProboxAM/SparkEngine/blob/master/docs/images/editorgif.gif)

* In-game gif
![eg](https://github.com/ProboxAM/SparkEngine/blob/master/docs/images/gamegif.gif)

* Innovation Animation Controller editor gif
![eg](https://github.com/ProboxAM/SparkEngine/blob/master/docs/images/innovation_02.gif)

* Innovation Key viewer
![eg](https://github.com/ProboxAM/SparkEngine/blob/master/docs/images/innovation_03.jpg)

* Innovation Manage clips gif
![eg](https://github.com/ProboxAM/SparkEngine/blob/master/docs/images/innovation_01.gif)

## Video

<iframe width="427" height="240" src="https://youtu.be/w7u863bj8Dc" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>

## Repository

- Organization's Github: [ProboxAM](https://github.com/ProboxAM)
- GitHub repository: [SparkEngine](https://github.com/ProboxAM/SparkEngine)

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
