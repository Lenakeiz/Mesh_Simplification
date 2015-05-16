#ifndef UI_SYS_H
#define UI_SYS_H

#include "../../octet.h"
namespace octet
{
  class UI_sys : public resource
  {      
    ref<visual_scene> UI_scene;
    ref<scene_node> UI_node;
    app *Program;
    std::vector<ref<UI_element>> Elements;
    int vx,vy;   
    int old_vx,old_vy;
    ref<material> mat;
    param_shader* sh;
   

    void render() {
      camera_instance *cam = UI_scene->get_camera_instance(0);
      cam->get_node()->loadIdentity();
      cam->set_ortho(vx, vy, 1, 1, -5);
      UI_scene->render((float)vx / vy);
    }

    void element_resized(int num)
    {       
      vec2 box_size = vec2(Elements[num]->Get_size());
      vec2 uv_bot_left, uv_top_right;
      Elements[num]->Get_texture_UV(uv_bot_left, uv_top_right);
      int size = sizeof(float) * 5;
      const GLfloat vertData[] = {
        -box_size.x(), box_size.y(), 0.0f,
        uv_bot_left.x(), uv_top_right.y(),

        box_size.x(), box_size.y(), 0.0f,
        uv_top_right.x(), uv_top_right.y(),

        -box_size.x(), -box_size.y(), 0.0f,
        uv_bot_left.x(), uv_bot_left.y(),

        box_size.x(), -box_size.y(), 0.0f,
        uv_top_right.x(), uv_bot_left.y()
      };

      {
        octet::gl_resource::wolock vt(Elements[num]->Get_mesh()->get_vertices());
        float* vtx = (float*)vt.u8();
        memcpy(vtx, &vertData[0], size * 4);
      }
      Elements[num]->resizecomplete();
    }

    void element_moved(int num)
    {
      vec3 box_pos = vec3(Elements[num]->Get_pos());      
      box_pos = vec3(box_pos.x() - vx / 2, vy / 2 - box_pos.y(), box_pos.z());
      Elements[num]->Get_node()->access_nodeToParent().loadIdentity();
      Elements[num]->Get_node()->access_nodeToParent().translate(box_pos);
      Elements[num]->move_complete();
    }
    void element_texture_changed(int num)
    {
      vec2 box_size = vec2(Elements[num]->Get_size());
      vec2 uv_bot_left, uv_top_right;
      Elements[num]->Get_texture_UV(uv_bot_left, uv_top_right);
      int size = sizeof(float) * 5;
      const GLfloat vertData[] = {
        -box_size.x(), box_size.y(), 0.0f,
        uv_bot_left.x(), uv_top_right.y(),

        box_size.x(), box_size.y(), 0.0f,
        uv_top_right.x(), uv_top_right.y(),

        -box_size.x(), -box_size.y(), 0.0f,
        uv_bot_left.x(), uv_bot_left.y(),

        box_size.x(), -box_size.y(), 0.0f,
        uv_top_right.x(), uv_bot_left.y()
      };
      {
        octet::gl_resource::wolock vt(Elements[num]->Get_mesh()->get_vertices());
        float* vtx = (float*)vt.u8();
        memcpy(vtx, &vertData[0], size * 4);
      }
      Elements[num]->texturechange_complete();
    }

    void screen_resized()
    {
      for (int i=0;i<Elements.size();i++)
      {     
        vec3 box_pos = vec3(Elements[i]->Get_pos());
        vec2 box_size = vec2(Elements[i]->Get_size());
        Elements[i]->Set_pos(vec2(box_pos.x()*vx/old_vx,box_pos.y()*vy/old_vy));
        Elements[i]->Set_size(vec2(box_size.x()*vx / old_vx, box_size.y()*vy / old_vy));
      }
    }
    public :
      void init(int _vx, int _vy,app *prog, string texture_string = "",string texture_mask="")
      {
        vx = _vx;
        vy = _vy;
        old_vx=_vx;
        old_vy = _vy;
        Program = prog;

        UI_scene = new visual_scene();
        UI_node = UI_scene->add_scene_node();
        UI_scene->create_default_camera_and_lights();
        camera_instance *cam = UI_scene->get_camera_instance(0);
        cam->get_node()->loadIdentity();   
        
        
        if (texture_string.size() > 0)
        {   
          sh = new param_shader("shaders/default.vs", "src/examples/UI_Tool/UI_Texture.fs");
          mat = new material(new image(texture_string), NULL, sh);
          mat->add_sampler(1, app_utils::get_atom("diffuse_mask"), new image(texture_mask), new sampler());
        }
        else
        {          
          sh = new param_shader("shaders/default.vs", "shaders/default_solid.fs");
          mat = new material(vec4(1, 0, 0, 1),sh);          
        }
      }

      void add_element(UI_element *new_ui_element)
      {
        vec3 box_pos = vec3(new_ui_element->Get_pos());
        vec2 box_size = vec2(new_ui_element->Get_size());
        vec2 uv_bot_left, uv_top_right;
        new_ui_element->Get_texture_UV(uv_bot_left, uv_top_right);
        box_pos = vec3(box_pos.x() - vx / 2, vy / 2 - box_pos.y(), box_pos.z()); // world 00 is at the center at screen 00 is at top left

        scene_node* element_node = new scene_node;
        new_ui_element->Set_node(element_node);      
        
        ref<mesh>  mesh_element = new mesh();    
        new_ui_element->Set_mesh(mesh_element);
        int size = sizeof(float) * 5;
        mesh_element->allocate(size * 4, 0);
        mesh_element->set_params(size, 0, 4, GL_TRIANGLE_STRIP, NULL);
        mesh_element->clear_attributes();
        mesh_element->add_attribute(octet::attribute_pos, 3, GL_FLOAT, 0);
        mesh_element->add_attribute(octet::attribute_uv, 2, GL_FLOAT, 12);

        const GLfloat vertData[] = {
          -box_size.x(), box_size.y(), 0.0f,
          uv_bot_left.x(), uv_top_right.y(),

          box_size.x(), box_size.y(), 0.0f,
          uv_top_right.x(), uv_top_right.y(),

          -box_size.x(), -box_size.y(), 0.0f,
          uv_bot_left.x(), uv_bot_left.y(),

          box_size.x(), -box_size.y(), 0.0f,
          uv_top_right.x(), uv_bot_left.y()
        };

        { 
          octet::gl_resource::wolock vt(mesh_element->get_vertices());
          float* vtx = (float*)vt.u8();
          memcpy(vtx, &vertData[0], size * 4);
        }

       
        element_node->access_nodeToParent().translate(box_pos);

        UI_scene->add_child(element_node);
        UI_scene->add_mesh_instance(new mesh_instance(element_node,mesh_element,mat));        
        Elements.push_back(new_ui_element);
      }

      void update(int _vx,int _vy)
      {
        if (vx != _vx || vy !=_vy)
        {
          vx=_vx;
          vy=_vy;
          screen_resized();
          old_vx=_vx;
          old_vy=_vy;
        }
          render();

          int m_pos_x,m_pos_y;
          Program->get_mouse_pos(m_pos_x,m_pos_y);         

          for (int i=0;i<Elements.size();i++)
          {
            if (Elements[i]->Get_is_active())
            {
            vec3 element_pos = Elements[i]->Get_pos();
            vec2 element_size = Elements[i]->Get_size();
            if (m_pos_x >= element_pos.x() - element_size.x() && m_pos_x <= element_pos.x() + element_size.x()
                && 
                m_pos_y >= element_pos.y() - element_size.y() && m_pos_y <= element_pos.y() + element_size.y()
                )
            {
              Elements[i]->OnHover();
            }
          }
          for (int i = 0; i<Elements.size(); i++)
          {
            if (Elements[i]->Get_is_active())
            {
            vec3 element_pos = Elements[i]->Get_pos();
            vec2 element_size = Elements[i]->Get_size();
            if (m_pos_x >= element_pos.x() - element_size.x() && m_pos_x <= element_pos.x() + element_size.x()
              &&
              m_pos_y >= element_pos.y() - element_size.y() && m_pos_y <= element_pos.y() + element_size.y()
              &&
              Program->is_key_going_down(key::key_lmb)
              )
            {
              Elements[i]->OnClick();
            }
            }
          }
          }
          
          for (int i = 0; i<Elements.size(); i++)
          {
            if (Elements[i]->is_moved())
            {
              element_moved(i);
            }
            if (Elements[i]->is_resized())
            {
              element_resized(i);
            }
            if (Elements[i]->is_texturechnaged())
            {
              element_texture_changed(i);
            }
          }
      }
  };
}
#endif