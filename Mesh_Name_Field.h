#ifndef UI_Mesh_Name_Field_H
#define UI_Mesh_Name_Field_H
namespace octet
{
  class Mesh_Name_Field : public UI_element
  {
  bool m_bOnFocus;
  public:
    Mesh_Name_Field(vec2 _pos, vec2 _size, float _depth, vec2 uv_bot_left, vec2 uv_top_right) : UI_element(_pos, _size, _depth, uv_bot_left, uv_top_right)
    {
      m_bOnFocus = false;
    }

    void OnClick()
    {
      m_bOnFocus = true;
    }

    bool IsMeshStringOnFocus() const
    {
      return m_bOnFocus;
    }
    void SetMeshStringOnFocus(bool bfocus)
    {
      m_bOnFocus = bfocus;
    }
    void OnHover()
    {

    }
  };
}
#endif