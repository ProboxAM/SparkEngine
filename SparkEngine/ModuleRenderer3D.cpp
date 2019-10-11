#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "Texture.h"
#include "glew\glew.h"
#include "SDL\include\SDL_opengl.h"

#define PAR_SHAPES_IMPLEMENTATION
#include "Par/par_shapes.h"
#include "ModuleImporter.h"
#include "Mesh.h"

#define ILUT_USE_OPENGL
#include "DeviL/include/ilut.h"


#include "ModuleRenderer3D.h"

#pragma comment (lib, "glew/glew32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */


ModuleRenderer3D::ModuleRenderer3D(bool start_enabled) : Module(start_enabled)
{
	name = "Render";
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init(nlohmann::json::iterator it)
{
	LOG("Creating 3D Renderer context");
	bool ret = true;

	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if(context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	if(ret == true)
	{
		//Use Vsync
		vsync = (*it)["vsync"];
		if(vsync && SDL_GL_SetSwapInterval(vsync) < 0)
			LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", __glewErrorStringREGAL(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", __glewErrorStringREGAL(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", __glewErrorStringREGAL(error));
			ret = false;
		}
		
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		lights[0].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);
	}

	// Projection matrix for
	OnResize(App->window->GetWindowWidth(), App->window->GetWindowHeight());

	GLenum err = glewInit();
	// … check for errors
	LOG("Using Glew %s", glewGetString(GLEW_VERSION));
	LOG("Vendor: %s", glGetString(GL_VENDOR));
	LOG("Renderer: %s", glGetString(GL_RENDERER));
	LOG("OpenGL version supported %s", glGetString(GL_VERSION));
	LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	//my_sphere = par_shapes_create_subdivided_sphere(5);

	/*my_vertex = 0;
	glGenBuffers(1, &(my_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*my_sphere->npoints * 3, my_sphere->points, GL_STATIC_DRAW);

	glGenBuffers(1, &(my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(PAR_SHAPES_T)*my_sphere->ntriangles*3, my_sphere->triangles, GL_STATIC_DRAW);*/

	//test_meshes = App->importer->LoadFBXFile("Assets/Warrior.fbx");

	GLubyte checkImage[48][48][4];
	for (int i = 0; i < 48; i++)
	{ 
		for (int j = 0; j < 48; j++)
		{ 
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;  
			checkImage[i][j][0] = (GLubyte)c;  
			checkImage[i][j][1] = (GLubyte)c;  
			checkImage[i][j][2] = (GLubyte)c;  
			checkImage[i][j][3] = (GLubyte)255; 
		} 
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 
	glGenTextures(1, &test_texture_id); 
	glBindTexture(GL_TEXTURE_2D, test_texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 48, 48, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);

	ILuint idd;
	ilGenImages(1, &idd); // Grab a new image name.
	ilBindImage(idd);
	ilLoadImage("Lenna_test_image.png");
	test_texture_lena_id = ilutGLBindTexImage();
	glBindTexture(GL_TEXTURE_2D, 0);
	ilDeleteImages(1, &idd);

	return ret;
}
// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	//// Seed with a real random value, if available
	//pcg_extras::seed_seq_from<std::random_device> seed_source;
	//// Make a random number engine
	//pcg32 rng(seed_source);

	//// Choose a random mean between 1 and 6
	//std::uniform_real_distribution<float> uniform_dist(0, 1);
	//float mean = uniform_dist(rng);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());


	// light 0 on cam pos
	lights[0].SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();


	//Par_shapes--------------------------------------------------------------------------------------------------------------------//

	/*glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	//// … draw other buffers
	glDrawElements(GL_TRIANGLES, my_sphere->ntriangles * 3, GL_UNSIGNED_SHORT, NULL);
	glDisableClientState(GL_VERTEX_ARRAY);*/

	//Meshes Test--------------------------------------------------------------------------------------------------------------------//

	for (std::vector<Mesh>::iterator it = test_meshes.begin(); it != test_meshes.end(); ++it)
	{
		DrawMesh(*it);
		DebugVertexNormals(*it);
	}

	for (std::vector<Mesh>::iterator it = test_meshes_dropped.begin(); it != test_meshes_dropped.end(); ++it)
	{
		DrawMesh(*it);
		DebugVertexNormals(*it);
	}

	//Index Mode--------------------------------------------------------------------------------------------------------------------//

	//float vertices[] = {
	//	0.f, 0.f, 0.f,
	//	-5.f, 0.f, 0.f,
	//	-5.f, -5.f, 0.f,
	//	0, -5.f, 0,
	//	0, -5, -5,
	//	0, 0, -5,
	//	-5, 0, -5,
	//	-5, -5, -5 };
	//int indices[] = { 0,1,2, 2,3,0,   // 36 of indices
	//				 0,3,4, 4,5,0,
	//				 0,5,6, 6,1,0,
	//				 1,6,7, 7,2,1,
	//				 7,4,3, 3,2,7,
	//				 4,7,6, 6,5,4 };
	//uint my_id = 0;
	//glGenBuffers(1, (GLuint*) &(my_id));
	//glBindBuffer(GL_ARRAY_BUFFER, my_id);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float)*8 * 3, vertices, GL_STATIC_DRAW);
	//uint my_indices = 0;
	//glGenBuffers(1, (GLuint*) &(my_indices));
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint)*36, indices, GL_STATIC_DRAW);

	//glEnableClientState(GL_VERTEX_ARRAY);
	//glVertexPointer(3, GL_FLOAT, 0, NULL);
	//// … draw other buffers
	//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
	//glDisableClientState(GL_VERTEX_ARRAY);

	//Direct Mode--------------------------------------------------------------------------------------------------------------------//

	//CUBO
	/*glBegin(GL_TRIANGLES);

	// front face =================
	glVertex3f(0.f, 0.f, 0.f);    // v0-v1-v2
	glVertex3f(-5.f, 0.f, 0.f);
	glVertex3f(-5.f, -5.f, 0.f);

	glVertex3f(-5.f, -5.f, 0);    // v2-v3-v0
	glVertex3f(0, -5.f, 0);
	glVertex3f(0, 0.f, 0);

	// right face =================
	glVertex3f(0, 0, 0);    // v0-v3-v4
	glVertex3f(0, -5, 0);
	glVertex3f(0, -5, -5);

	glVertex3f(0, -5, -5);    // v4-v5-v0
	glVertex3f(0, 0, -5);
	glVertex3f(0, 0, 0);

	// top face ===================
	glVertex3f(0, 0, 0);    // v0-v5-v6
	glVertex3f(0, 0, -5);
	glVertex3f(-5, 0, -5);

	glVertex3f(-5, 0, -5);    // v6-v1-v0
	glVertex3f(-5, 0, 0);
	glVertex3f(0, 0, 0);

	// left face
	glVertex3f(-5, 0, 0);    // v0-v5-v6
	glVertex3f(-5, 0, -5);
	glVertex3f(-5, -5, -5);

	glVertex3f(-5, -5, -5);    // v0-v5-v6
	glVertex3f(-5, -5, 0);
	glVertex3f(-5, 0, 0);

	// bottom face
	glVertex3f(-5, -5, 0);    // v0-v5-v6
	glVertex3f(-5, -5, -5);
	glVertex3f(0, -5, -5);

	glVertex3f(0, -5, -5);    // v0-v5-v6
	glVertex3f(0, -5, 0);
	glVertex3f(-5, -5, 0);

	// back face
	glVertex3f(-5, 0.f, -5);    // v0-v1-v2
	glVertex3f(0, 0.f, -5);
	glVertex3f(0, -5, -5);

	glVertex3f(0, -5, -5);
	glVertex3f(-5.f, -5.f, -5);    // v2-v3-v0
	glVertex3f(-5, 0.f, -5);*/
	glBindTexture(GL_TEXTURE_2D, test_texture_lena_id);
	glBegin(GL_QUADS);
	// Front Face
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);  // Bottom Left Of The Texture and Quad
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);  // Bottom Right Of The Texture and Quad
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);  // Top Right Of The Texture and Quad
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);  // Top Left Of The Texture and Quad
															  // Back Face
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);  // Bottom Right Of The Texture and Quad
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);  // Top Right Of The Texture and Quad
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);  // Top Left Of The Texture and Quad
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);  // Bottom Left Of The Texture and Quad
															   // Top Face
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);  // Top Left Of The Texture and Quad
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f);  // Bottom Left Of The Texture and Quad
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 1.0f);  // Bottom Right Of The Texture and Quad
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);  // Top Right Of The Texture and Quad
															  // Bottom Face
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);  // Top Right Of The Texture and Quad
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, -1.0f, -1.0f);  // Top Left Of The Texture and Quad
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);  // Bottom Left Of The Texture and Quad
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);  // Bottom Right Of The Texture and Quad
															   // Right face
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);  // Bottom Right Of The Texture and Quad
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);  // Top Right Of The Texture and Quad
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);  // Top Left Of The Texture and Quad
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);  // Bottom Left Of The Texture and Quad
															  // Left Face
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);  // Bottom Left Of The Texture and Quad
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);  // Bottom Right Of The Texture and Quad
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);  // Top Right Of The Texture and Quad
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);  // Top Left Of The Texture and Quad
	//glEnd();
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	App->editor->Draw();
	SDL_GL_SwapWindow(App->window->window);
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	return true;
}


void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

bool ModuleRenderer3D::Save(nlohmann::json &it)
{
	it[name] = {
		{"vsync",vsync}
	};

	return true;
}

void ModuleRenderer3D::GLEnable(unsigned int flag, bool active)
{
	if (active)
		glEnable(flag);
	else
		glDisable(flag);
}


void ModuleRenderer3D::DrawMesh(Mesh m)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, m.buffers[BUFF_VERT]);
	glVertexPointer(3, GL_FLOAT, 0, nullptr);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.buffers[BUFF_IND]);

	glBindBuffer(GL_ARRAY_BUFFER, m.buffers[BUFF_NORM]);
	glNormalPointer(GL_FLOAT, 0, nullptr);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, m.buffers[BUFF_UV]);
	glTexCoordPointer(2, GL_FLOAT, 0, nullptr);
	glBindTexture(GL_TEXTURE_2D, m.tex->id);

	glDrawElements(GL_TRIANGLES, m.indices.size(), GL_UNSIGNED_INT, nullptr);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void ModuleRenderer3D::SetWireframeMode(bool on)
{
	if(on)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void ModuleRenderer3D::DebugVertexNormals(Mesh m)
{
/*	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, m.dg_nm_vbo); 
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawArrays(GL_LINES, 0, m.debug_normals.size());
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);*/
}