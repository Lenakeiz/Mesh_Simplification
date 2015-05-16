#ifndef UI_element_H
#define UI_element_H

namespace octet
{
  class UI_element : public resource
  {
      vec2 pos;
      vec2 size;
      float depth;
      vec2 UV_bot_left;
      vec2 UV_top_right;

      scene_node *node;
      ref<mesh> ui_mesh;
     
      bool is_active;
      bool resized;
      bool moved;
      bool texturechanged;
      public :

        UI_element(vec2 _pos, vec2 _size,float _depth,vec2 _UV_tl, vec2 _UV_br)
        {   
          is_active = true;
          resized=false;
          moved = false;
          texturechanged=false;
          depth = _depth;
          pos = _pos;
          size = _size;
         
          UV_bot_left = _UV_tl;
          UV_top_right = _UV_br;
        }

        UI_element()
        {
        
        }
        virtual ~UI_element()
        {
          delete ui_mesh;
          delete node;
        }

        mesh* Get_mesh() const
        {
          return ui_mesh;
        }
        void Set_mesh(mesh *new_mesh)
        {
          ui_mesh=new_mesh;
        }
        void Set_pos(vec2 _pos)
        {
          pos=_pos;
          moved =true;
        }
        void Set_Depth(float _depth)
        {
          depth = _depth;
          moved = true;
        }

        vec3 Get_pos() const
        {
          return vec3(pos.x(),pos.y(),depth);
        }


        void Set_size(vec2 _size)
        {
          size=_size;
          resized=true;
        }
        vec2 Get_size() const
        {
          return size;
        }


        void Set_node(scene_node *_node)
        {
          node=_node;
        }
        scene_node* Get_node() const
        {
          return node;
        }

        void Set_texture_UV(vec2 _UV_bl, vec2 _UV_tr)
        {
          UV_bot_left = _UV_bl;
          UV_top_right = _UV_tr;
          texturechanged=true;
        }

        void Get_texture_UV(vec2 &_UV_bl, vec2 &_UV_tr) const
        {
          _UV_bl = UV_bot_left;
          _UV_tr = UV_top_right;
        }
        
        void Set_is_active(bool _isactive)
        {
          is_active = _isactive;
        }

        bool Get_is_active() const 
        {
          return is_active;
        }
        bool is_resized() const
        {
          return resized;
        }
        void resizecomplete()
        {
          resized=false;
        }
        bool is_moved() const
        {
          return moved;
        }
        void move_complete()
        {
          moved=false;
        }
        bool is_texturechnaged() const
        {
          return texturechanged;
        }
        void texturechange_complete()
        {
          texturechanged=false;
        }
        virtual void OnClick()=0;
        virtual void OnHover()=0;

        
  };
}
#endif