# SparkEngine
Spark Engine is a 3D engine developed by Marc Guillen and Axel Alavedra during the Game Development grade at CITM-UPC Terrassa.

- Axel's GitHub account: [AxelAlavedra](https://github.com/AxelAlavedra)
- Marc's GitHub account: [Marcgs96](https://github.com/Marcgs96)
- Organization's Github: [ProboxAM](https://github.com/ProboxAM)
- GitHub repository: [SparkEngine](https://github.com/ProboxAM/SparkEngine)

### Features

#### Game Objects
- Drag fbx creates a new Game Object with mesh and texture.
- Drag texture with Game Object selected applies the texture to it.
- GameObject menu to create primitive shapes (Cube doesn't have uv/normals).
- Activate/Deactivate Game Object components on Inspector.

#### Debug
- Component Mesh on Inspector can activate debug vertex normals and face normals.
- Component Texture on Inspector can activate Checkers texture for the GameObject.

### Controls

#### Camera
- Mouse Hold Right:
	- WASD: move forward, left, backward and right
	- Look around
- F: look at target
- Hold Alt:
	- Mouse Hold Left: look around target
- Mouse Wheel: zoom in/out
- Mouse Hold Shift: move faster

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