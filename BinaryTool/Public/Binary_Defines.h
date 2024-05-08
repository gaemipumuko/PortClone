#pragma once

#include "Process.h"
#include <iostream>

namespace Binary
{
}

/* Mesh (Assimp) */
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#ifdef _DEBUG
#pragma comment(lib, "Assimp/assimp-vc143-mtd.lib")
#else
#pragma comment(lib, "Assimp/assimp-vc143-mt.lib")
#endif

using namespace		Binary;