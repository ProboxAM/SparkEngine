#include "Application.h"
#include "ModuleWindow.h"
#include "ResourceTexture.h"
#include "glew\glew.h"
#include "SDL\include\SDL_opengl.h"
#include "MathGeoLib/Math/float4x4.h"
#include "ModuleCamera3D.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "ModuleImporter.h"
#include "ResourceMesh.h"
#include "ComponentCamera.h"
#include "ComponentMesh.h"
#include "ModuleRenderer3D.h"

#pragma comment (lib, "glew/glew32.lib")    /* link OpenGL Utility lib  */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */


ModuleRenderer3D::ModuleRenderer3D(bool start_enabled) : Module(start_enabled)
{
	name = "Render";
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init(const nlohmann::json::iterator &it)
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
		if(SDL_GL_SetSwapInterval(vsync) < 0)
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
		
		Load(it);

		lights[0].Active(true);
	}

	GLenum err = glewInit();
	if (err != GL_NO_ERROR) // … check for errors
	{
		LOG("Failed glew Init with error: %s", SDL_GetError());
		ret = false;
	}
	else
	{
		LOG("Using Glew %s", glewGetString(GLEW_VERSION));
		LOG("Vendor: %s", glGetString(GL_VENDOR));
		LOG("Renderer: %s", glGetString(GL_RENDERER));
		LOG("OpenGL version supported %s", glGetString(GL_VERSION));
		LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

		// Projection matrix for
		OnResize(App->window->GetWindowWidth(), App->window->GetWindowHeight());

		CreateSceneBuffer();
		CreateGameBuffer();
	}

	return ret;
}
// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate()
{
	// light 0 on cam pos
	lights[0].SetPos(editor_camera->frustum.pos.x, editor_camera->frustum.pos.y, editor_camera->frustum.pos.z);

	for (uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate()
{
	DrawSceneViewPort();
	if(game_camera)DrawGameViewPort();

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
	ResizeScene(width, height);
	ResizeGame(width, height);
}

bool ModuleRenderer3D::Load(const nlohmann::json::iterator& it)
{
	vsync = (*it)["vsync"];
	depth_test = (*it)["depth_test"];
	cull_face = (*it)["cull_face"];
	lighting = (*it)["lighting"];
	color_material = (*it)["color_material"];
	texture2d = (*it)["texture2d"];
	wireframe = (*it)["wireframe"];

	GLEnable(GL_DEPTH_TEST, depth_test);
	GLEnable(GL_CULL_FACE, cull_face);
	GLEnable(GL_LIGHTING, lighting);
	GLEnable(GL_COLOR_MATERIAL, color_material);
	GLEnable(GL_TEXTURE_2D, texture2d);
	SetWireframeMode(wireframe);

	return true;
}


bool ModuleRenderer3D::Save(nlohmann::json &it)
{
	it[name] = {
		{ "vsync",vsync },
		{ "depth_test",depth_test },
		{ "cull_face",cull_face },
		{ "lighting",lighting },
		{ "color_material",color_material },
		{ "texture2d",texture2d },
		{ "wireframe",wireframe}
	};

	return true;
}

//Enables or disables a GL configuration
void ModuleRenderer3D::GLEnable(unsigned int flag, bool active)
{
	switch (flag)
	{
	case GL_DEPTH_TEST: depth_test = active;
		break;
	case GL_CULL_FACE: cull_face = active;
		break;
	case GL_LIGHTING: lighting = active;
		break;
	case GL_COLOR_MATERIAL: color_material = active;
		break;
	case GL_TEXTURE_2D: texture2d = active;
		break;
	}

	if (active)
		glEnable(flag);
	else
		glDisable(flag);
}

//Returns if a GL config is enabled or not
bool ModuleRenderer3D::IsEnabled(unsigned int flag) const
{
	bool ret = false;
	switch (flag)
	{
	case GL_DEPTH_TEST: ret = depth_test;
		break;
	case GL_CULL_FACE: ret = cull_face;
		break;
	case GL_LIGHTING: ret = lighting;
		break;
	case GL_COLOR_MATERIAL: ret = color_material;
		break;
	case GL_TEXTURE_2D: ret = texture2d;
		break;
	}
	return ret;
}

//Draws a mesh and binds texture to it
void ModuleRenderer3D::DrawMesh(const ResourceMesh* m, const ResourceTexture* tex, const float4x4& mtransform) const
{
	glPushMatrix();
	glMultMatrixf((float*)&mtransform.Transposed());

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, m->buffers[BUFF_VERT]);
	glVertexPointer(3, GL_FLOAT, 0, nullptr);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->buffers[BUFF_IND]);

	if (m->total_normal > 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m->buffers[BUFF_NORM]);
		glNormalPointer(GL_FLOAT, 0, nullptr);
	}
	if (m->total_uv > 0)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, m->buffers[BUFF_UV]);
		glTexCoordPointer(2, GL_FLOAT, 0, nullptr);
	}
	if(tex)
		glBindTexture(GL_TEXTURE_2D, tex->buffer_id);

	glDrawElements(GL_TRIANGLES, m->total_indices, GL_UNSIGNED_INT, nullptr);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();
}

void ModuleRenderer3D::SetWireframeMode(bool on)
{
	wireframe = on;
	if(on)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

bool ModuleRenderer3D::IsWireframeEnabled() const {
	return wireframe;
}

void ModuleRenderer3D::DebugVertexNormals(const ResourceMesh* m, const float4x4& mtransform) const
{
	/*glPushMatrix();
	glMultMatrixf((float*)&mtransform.Transposed());

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, m->buffers[BUFF_DEBUG_VERT_NORM]); 
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawArrays(GL_LINES, 0, m->debug_vertex_normals.size());
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glPopMatrix();*/
}

void ModuleRenderer3D::DebugFaceNormals(const ResourceMesh* m, const float4x4& mtransform) const
{
	/*glPushMatrix();
	glMultMatrixf((float*)&mtransform.Transposed());

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, m->buffers[BUFF_DEBUG_FACE_NORM]);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawArrays(GL_LINES, 0, m->debug_face_normals.size());
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glPopMatrix();*/
}

void ModuleRenderer3D::DebugDrawCube(const float3 * vertices, Color color) const
{
	glEnableClientState(GL_CULL_FACE);
	glEnableClientState(GL_LIGHTING);
	glEnableClientState(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glColor3f(color.r, color.g, color.b);
	glLineWidth(5.0f);

	glBegin(GL_QUADS);

	glVertex3fv((const GLfloat*)&vertices[1]);
	glVertex3fv((const GLfloat*)&vertices[5]);
	glVertex3fv((const GLfloat*)&vertices[7]);
	glVertex3fv((const GLfloat*)&vertices[3]);

	glVertex3fv((const GLfloat*)&vertices[4]);
	glVertex3fv((const GLfloat*)&vertices[0]);
	glVertex3fv((const GLfloat*)&vertices[2]);
	glVertex3fv((const GLfloat*)&vertices[6]);

	glVertex3fv((const GLfloat*)&vertices[5]);
	glVertex3fv((const GLfloat*)&vertices[4]);
	glVertex3fv((const GLfloat*)&vertices[6]);
	glVertex3fv((const GLfloat*)&vertices[7]);

	glVertex3fv((const GLfloat*)&vertices[0]);
	glVertex3fv((const GLfloat*)&vertices[1]);
	glVertex3fv((const GLfloat*)&vertices[3]);
	glVertex3fv((const GLfloat*)&vertices[2]);

	glVertex3fv((const GLfloat*)&vertices[3]);
	glVertex3fv((const GLfloat*)&vertices[7]);
	glVertex3fv((const GLfloat*)&vertices[6]);
	glVertex3fv((const GLfloat*)&vertices[2]);

	glVertex3fv((const GLfloat*)&vertices[0]);
	glVertex3fv((const GLfloat*)&vertices[4]);
	glVertex3fv((const GLfloat*)&vertices[5]);
	glVertex3fv((const GLfloat*)&vertices[1]);

	glEnd();

	glColor3f(1.f, 1.f, 1.f);
	glLineWidth(1.0f);

	glDisableClientState(GL_CULL_FACE);
	glDisableClientState(GL_LIGHTING);
	glDisableClientState(GL_TEXTURE_2D);
	App->renderer3D->SetWireframeMode(wireframe);
}

void ModuleRenderer3D::DebugDrawLines(std::vector<float3> lines)
{
	glColor3f(0.f, 1.0f, 0.0f);
	glBegin(GL_LINES);
	glLineWidth(5.0f);

	for (int i = 0; i < lines.size(); i++)
	{
		glVertex3f((GLfloat)lines[i].x, (GLfloat)lines[i].y, (GLfloat)lines[i].z);
	}

	glLineWidth(1.0f);
	glColor3f(1.f, 1.f, 1.f);
	glEnd();
}

void ModuleRenderer3D::DrawOutline(const ResourceMesh * m, const Color color, const float4x4 & mtransform) const
{
	glColor3f((GLfloat)color.r, (GLfloat)color.g, (GLfloat)color.b);
	glLineWidth(2.f);

	glPolygonMode(GL_FRONT, GL_LINE);

	glPushMatrix();
	glMultMatrixf((float*)&mtransform.Transposed());

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, m->buffers[BUFF_VERT]);
	glVertexPointer(3, GL_FLOAT, 0, nullptr);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->buffers[BUFF_IND]);

	glDrawElements(GL_TRIANGLES, m->total_indices, GL_UNSIGNED_INT, nullptr);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor3f(1.f, 1.f, 1.f);
	glLineWidth(1.f);

	glPopMatrix();
}

//Resize texture buffer of Scene rendering
void ModuleRenderer3D::ResizeScene(float w, float h)
{
	glBindTexture(GL_TEXTURE_2D, scene_texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, scene_depth_id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
}

void ModuleRenderer3D::ResizeGame(float w, float h)
{
	glBindTexture(GL_TEXTURE_2D, game_texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, game_depth_id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
}

void ModuleRenderer3D::UpdateSceneProjectionMatrix()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glLoadMatrixf((float*)&editor_camera->GetOpenGLProjectionMatrix());
}

void ModuleRenderer3D::UpdateGameProjectionMatrix()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glLoadMatrixf((float*)&game_camera->GetOpenGLProjectionMatrix());
}

//Creates a Frame Buffer for rendering into Scene window
void ModuleRenderer3D::CreateSceneBuffer()
{
	glGenFramebuffers(1, &scene_buffer_id);
	glBindFramebuffer(GL_FRAMEBUFFER, scene_buffer_id);

	glGenTextures(1, &scene_texture_id);
	glBindTexture(GL_TEXTURE_2D, scene_texture_id);

	float2 size = float2(App->window->GetWindowWidth(), App->window->GetWindowHeight());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, scene_texture_id, 0);
	glBindTexture(GL_TEXTURE_2D, 0);	

	glGenRenderbuffers(1, &scene_depth_id);
	glBindRenderbuffer(GL_RENDERBUFFER, scene_depth_id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.x, size.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, scene_depth_id);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ModuleRenderer3D::CreateGameBuffer()
{
	glGenFramebuffers(1, &game_buffer_id);
	glBindFramebuffer(GL_FRAMEBUFFER, game_buffer_id);

	glGenTextures(1, &game_texture_id);
	glBindTexture(GL_TEXTURE_2D, game_texture_id);

	float2 size = float2(App->window->GetWindowWidth(), App->window->GetWindowHeight());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, game_texture_id, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &game_depth_id);
	glBindRenderbuffer(GL_RENDERBUFFER, game_depth_id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.x, size.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, game_depth_id);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool ModuleRenderer3D::GetVsync() const
{
	return vsync;
}
void ModuleRenderer3D::SetVsync(bool active)
{
	vsync = active;
	SDL_GL_SetSwapInterval(active);
}

void ModuleRenderer3D::DrawSceneViewPort()
{

	UpdateSceneProjectionMatrix();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glLoadMatrixf((float*)&editor_camera->GetOpenGLViewMatrix());

	glBindFramebuffer(GL_FRAMEBUFFER, scene_buffer_id); //set scene buffer to render to a texture
	glClearColor(bkg_color.x, bkg_color.y, bkg_color.z, 1.0); // background color for scene
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (game_camera && game_camera->enable_frustum_culling) App->scene->AccelerateFrustumCulling(game_camera);
	else App->scene->AccelerateFrustumCulling(editor_camera);

	App->scene->Draw(); //Draw scene
	App->scene->DebugDraw();//Draw Quadtree and Grid

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default draw
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ModuleRenderer3D::DrawGameViewPort()
{
	UpdateGameProjectionMatrix();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glLoadMatrixf((float*)&game_camera->GetOpenGLViewMatrix());

	glBindFramebuffer(GL_FRAMEBUFFER, game_buffer_id); //set scene buffer to render to a texture
	glClearColor(bkg_color.x, bkg_color.y, bkg_color.z, 1.0); // background color for scene
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	App->scene->AccelerateFrustumCulling(game_camera);
	App->scene->Draw(); //Draw scene

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default draw
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

