#pragma once


#include <d3d9.h>
#include <d3dx9.h>

#include <objidl.h>
#include <gdiplus.h> 

#include "../Common/common.h"
using namespace common;

//#include "../wxMemMonitorLib/wxMemMonitor.h" // debug library

static const StrPath g_defaultTexture = "white.dds";


#include "../ai/ai.h"

#include "textdesigner\PngOutlineText.h"


#include "base/graphicdef.h"
#include "model/rawmesh.h"
#include "model/rawani.h"
#include "collision/boundingbox.h"
#include "collision/boundingbox2.h"
#include "collision/boundingsphere.h"

#include "interface/pickable.h"
#include "interface/renderable.h"
#include "interface/shaderrenderer.h"
#include "interface/shadowrenderer.h"

#include "model/node.h"
#include "model/node2.h"

#include "base/color.h"
#include "base/fontgdi.h"
#include "base/viewport.h"
#include "base/material.h"
#include "base/light.h"
#include "base/vertexdeclaration.h"
#include "base/vertexbuffer.h"
#include "base/indexbuffer.h"
#include "base/meshbuffer.h"
#include "base/texture.h"
#include "base/texturecube.h"
#include "base/vertexformat.h"
#include "base/grid.h"
#include "base/grid2.h"
#include "base/grid3.h"
#include "base/box.h"
#include "base/pyramid.h"
#include "base/line.h"
#include "base/line2.h"
#include "base/line3.h"
#include "base/line2d.h"
#include "base/cube.h"
#include "base/cube2.h"
#include "base/cube3.h"
#include "base/cube4.h"
#include "base/shader.h"
#include "base/camera.h"
#include "base/skybox.h"
#include "base/skybox2.h"
#include "base/sphere.h"
#include "base/surface.h"
#include "base/surface2.h"
#include "base/surface3.h"
#include "base/circle.h"
#include "base/circleline.h"
#include "base/quad.h"
#include "base/quad2d.h"
#include "base/billboard.h"
#include "base/text.h"
#include "base/text3d.h"
#include "base/text3d2.h"
#include "base/text3d3.h"

#include "base/shadow1.h"
#include "base/shadow2.h"
#include "base/shadowmap.h"
#include "base/shadowvolume.h"

#include "collision/collision.h"
#include "collision/collisionmanager.h"
#include "collision/frustum.h"


#include "dbg/dbgbox.h"
#include "dbg/dbgbox2.h"
#include "dbg/dbgsphere.h"
#include "dbg/dbgline.h"
#include "dbg/dbglinelist.h"
#include "dbg/dbgarrow.h"
#include "dbg/dbgfrustum.h"
#include "dbg/dbgquad.h"
#include "dbg/dbgquad2.h"
#include "dbg/dbgaxis.h"

//#include "base/xfilemesh.h"



#include "particle/particles.h"
#include "particle/snow.h"


#include "model/track.h"
#include "model/blendtrack.h"
#include "model/mesh.h"
#include "model/rigidmesh.h"
#include "model/skinnedmesh.h"
#include "model/model.h"
#include "model/bonemgr.h"
#include "model/bonenode.h"

#include "model_collada/skeleton.h"
#include "model_collada/animationnode.h"
#include "model_collada/animation.h"
#include "model_collada/mesh2.h"
#include "model_collada/model_collada.h"
#include "model_collada/colladaloader.h"


#include "model_new/xfilemesh.h"
#include "model_new/xfilemodel.h"
#include "model_new/model2.h"


#include "character/character.h"

#include "sprite/sprite.h"
#include "sprite/sprite2.h"

#include "terrain/rawterrain.h"
#include "terrain/water.h"
#include "terrain/terrain.h"
#include "terrain/terrain2.h"
#include "terrain/tile.h"
#include "terrain/terraincursor.h"
#include "terrain/terraineditor.h"
#include "terrain/terrainimporter.h"
#include "terrain/terrainexporter.h"
#include "terrain/terrainloader.h"


namespace graphic
{
	struct sRenderObj
	{
		int opt;
		Vector3 normal;
		Matrix44 tm;
		cNode2 *p;
	};

	struct sAlphaBlendSpace
	{
		cBoundingBox bbox;
		std::vector<sRenderObj> renders;
	};
}


#include "manager/textmanager.h"
#include "manager/Renderer.h"
#include "manager/resourcemanager.h"
#include "manager/maincamera.h"
#include "manager/lightmanager.h"
#include "manager/fontmanager.h"
#include "manager/pickmanager.h"

#include "importer/parallelloader.h"



#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "d3dx9.lib" )
#pragma comment( lib, "gdiplus.lib" ) 

#ifdef _DEBUG
	//#pragma comment( lib, "assimp-vc140-mt.lib" ) 
#else
	//#pragma comment( lib, "assimp-vc140-mt.lib" ) 
#endif
