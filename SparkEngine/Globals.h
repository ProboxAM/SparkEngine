#pragma once

// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 )

#include <windows.h>
#include <stdio.h>
#include <ctype.h>
#include "MathGeoLib\Math\float4.h"
#include "MathGeoLib\Math\float3.h"
#include "MathGeoLib\Math\float2.h"

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define HAVE_M_PI
#define DEBUG_NORMAL_LENGTH 0.2f

typedef unsigned int uint;

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Deletes a buffer
#define RELEASE( x )\
    {\
       if( x != nullptr )\
       {\
         delete x;\
	     x = nullptr;\
       }\
    }

// Deletes an array of buffers
#define RELEASE_ARRAY( x )\
	{\
       if( x != nullptr )\
       {\
           delete[] x;\
	       x = nullptr;\
		 }\
	 }

// Configuration -----------
#define VERSION "0.4-alpha"
#define ASSETS_FOLDER "Assets/"
#define SETTINGS_FOLDER "Settings/"
#define LIBRARY_FOLDER "Library/"
#define LIBRARY_AUDIO_FOLDER "Library/Audio/"
#define LIBRARY_TEXTURES_FOLDER "Library/Textures/"
#define LIBRARY_MESH_FOLDER "Library/Meshes/"
#define LIBRARY_ANIMATION_FOLDER "Library/Animations/"
#define LIBRARY_BONE_FOLDER "Library/Bones/"
#define LIBRARY_SCENE_FOLDER "Library/Scenes/"
#define LIBRARY_MODEL_FOLDER "Library/Models/"
#define LIBRARY_MATERIAL_FOLDER "Library/Materials/"
#define LIBRARY_STATE_MACHINE_FOLDER "Library/StateMachines/"
#define LIBRARY_ANIM_CONTROLLER_FOLDER "Library/AnimControllers/"
#define MESH_EXTENSION ".smesh"
#define MODEL_EXTENSION ".smodel"
#define TEXTURE_EXTENSION ".dds"
#define MATERIAL_EXTENSION ".smat"
#define SCENE_EXTENSION ".spk"
#define ANIM_EXTENSION ".sanim"
#define BONE_EXTENSION ".sbone"
#define ANIM_CONTROLLER_EXTENSION ".scontroller"