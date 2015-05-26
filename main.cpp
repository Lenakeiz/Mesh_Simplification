////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
// Text overlay
//

#define OCTET_BULLET 1

#include "../../octet.h"
#include"UI_element.h"
#include"UI_button.h"
#include"UI_Algo_start_Button.h"
#include"Mesh_Name_field.h"
#include"UI_sys.h"
#include "Obj_Importer.h"
#include"MeshSimplifier.h"
#include"EQMeshSimplifier.h"
#include"Mesh_Simplification.h"

/// Create a box with octet
int main(int argc, char **argv) {
  // set up the platform.
  octet::app::init_all(argc, argv);

  // our application.
  octet::Mesh_Simplification app(argc, argv);
  app.init();

  // open windows
  octet::app::run_all_apps();
}


