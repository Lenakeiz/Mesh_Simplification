#ifndef UI_startbutton_H
#define UI_startbutton_H
namespace octet
{
  class UI_Algo_start_Button : public UI_element
  {
    bool m_bSubmit =false;
  public:
    UI_Algo_start_Button(vec2 _pos, vec2 _size, float _depth, vec2 uv_bot_left, vec2 uv_top_right) : UI_element(_pos, _size, _depth, uv_bot_left, uv_top_right)
    {

    }

    bool IsSubmit() const
    {
      return m_bSubmit;
    }
    void SetSubmit(bool _submit)
    {
      m_bSubmit = _submit;
    }
    void OnClick()
    {
      m_bSubmit = true;
    }

    void OnHover()
    {

    }
  };
}
#endif