////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
namespace octet {
  /// Scene containing a box with octet.
  class Mesh_Simplification : public app {
    // scene for drawing box
    ref<visual_scene> app_scene;
    UI_sys GUI_sys;

  public:
    /// this is called when we construct the class before everything is initialised.
    Mesh_Simplification(int argc, char **argv) : app(argc, argv) {
    }

    /// this is called once OpenGL is initialized
    void app_init() {
      app_scene =  new visual_scene();
      app_scene->create_default_camera_and_lights();
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      GUI_sys.init(vx, vy, this, "assets/UI_sprite_sheet.gif", "assets/UI_sprite_sheet_mask.gif");
    }

    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);

      // update matrices. assume 30 fps.
      app_scene->update(1.0f/30);

      // draw the scene
      app_scene->render((float)vx / vy);
      GUI_sys.update(vx, vy);
    }
  };
}
