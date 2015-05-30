////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
#include "EQMeshSimplifier.h"
namespace octet {
  /// Scene containing a box with octet.



  class Mesh_Simplification : public app {
    // scene for drawing box
    ref<visual_scene> app_scene;
    UI_sys GUI_sys;
    UI_Algo_start_Button* m_pxSubmitButton;
    UI_Algo_start_Button* m_pxContinueButton;
    UI_Algo_start_Button* m_pxSaveMeshButton;
    Mesh_Name_Field* m_pxDeselect;

    string m_sMeshURL;
    int m_iTargetFaces;
    int m_iTargettri;
    float m_fVertProxi;
    float m_fMaxErrorth;
    int m_iSimplificationPerRun;

    int m_iFieldNumber;
    bool m_bMeshNameOnFocus = false;
    bool m_bDeselOnFocus = false;
    bool m_bFieldSubmit = false;
    bool m_bContinue = false;
    bool m_bSaveMesh = false;

    EQMeshSimplifier simples;
    Obj_Importer* importer;

    dynarray<string> m_sAlgoString;
    dynarray<Mesh_Name_Field*> m_pxUserInputFields;
    dynarray<int> m_xUserInputKeyCode;
    dynarray<string> m_xUserInputKeyValue;

    text_overlay *m_xoverlay;
    dynarray<mesh_text*> m_xMeshText;
    int m_iTextVX, m_iTextVY;

    bool wireframe;

    bool m_bContinueColourToggle = false;
    

    void FillInputKeyArray()
    {

      m_xUserInputKeyCode.push_back(13);
      m_xUserInputKeyValue.push_back(" ");//value is never used

      m_xUserInputKeyCode.push_back(8);
      m_xUserInputKeyValue.push_back(" ");//value is never used

      m_xUserInputKeyCode.push_back(48);
      m_xUserInputKeyValue.push_back("0");
      m_xUserInputKeyCode.push_back(49);
      m_xUserInputKeyValue.push_back("1");
      m_xUserInputKeyCode.push_back(50);
      m_xUserInputKeyValue.push_back("2");
      m_xUserInputKeyCode.push_back(51);
      m_xUserInputKeyValue.push_back("3");
      m_xUserInputKeyCode.push_back(52);
      m_xUserInputKeyValue.push_back("4");
      m_xUserInputKeyCode.push_back(53);
      m_xUserInputKeyValue.push_back("5");
      m_xUserInputKeyCode.push_back(54);
      m_xUserInputKeyValue.push_back("6");
      m_xUserInputKeyCode.push_back(55);
      m_xUserInputKeyValue.push_back("7");
      m_xUserInputKeyCode.push_back(56);
      m_xUserInputKeyValue.push_back("8");
      m_xUserInputKeyCode.push_back(57);
      m_xUserInputKeyValue.push_back("9");
      m_xUserInputKeyCode.push_back(190);
      m_xUserInputKeyValue.push_back(".");
      m_xUserInputKeyCode.push_back(20);
      m_xUserInputKeyValue.push_back(" ");

      m_xUserInputKeyCode.push_back(65);
      m_xUserInputKeyValue.push_back("a");
      m_xUserInputKeyCode.push_back(66);
      m_xUserInputKeyValue.push_back("b");
      m_xUserInputKeyCode.push_back(67);
      m_xUserInputKeyValue.push_back("c");
      m_xUserInputKeyCode.push_back(68);
      m_xUserInputKeyValue.push_back("d");
      m_xUserInputKeyCode.push_back(69);
      m_xUserInputKeyValue.push_back("e");
      m_xUserInputKeyCode.push_back(70);
      m_xUserInputKeyValue.push_back("f");
      m_xUserInputKeyCode.push_back(71);
      m_xUserInputKeyValue.push_back("g");
      m_xUserInputKeyCode.push_back(72);
      m_xUserInputKeyValue.push_back("h");
      m_xUserInputKeyCode.push_back(73);
      m_xUserInputKeyValue.push_back("i");
      m_xUserInputKeyCode.push_back(74);
      m_xUserInputKeyValue.push_back("j");
      m_xUserInputKeyCode.push_back(75);
      m_xUserInputKeyValue.push_back("k");
      m_xUserInputKeyCode.push_back(76);
      m_xUserInputKeyValue.push_back("l");
      m_xUserInputKeyCode.push_back(77);
      m_xUserInputKeyValue.push_back("m");
      m_xUserInputKeyCode.push_back(78);
      m_xUserInputKeyValue.push_back("n");
      m_xUserInputKeyCode.push_back(79);
      m_xUserInputKeyValue.push_back("o");
      m_xUserInputKeyCode.push_back(80);
      m_xUserInputKeyValue.push_back("p");
      m_xUserInputKeyCode.push_back(81);
      m_xUserInputKeyValue.push_back("q");
      m_xUserInputKeyCode.push_back(82);
      m_xUserInputKeyValue.push_back("r");
      m_xUserInputKeyCode.push_back(83);
      m_xUserInputKeyValue.push_back("s");
      m_xUserInputKeyCode.push_back(84);
      m_xUserInputKeyValue.push_back("t");
      m_xUserInputKeyCode.push_back(85);
      m_xUserInputKeyValue.push_back("u");
      m_xUserInputKeyCode.push_back(86);
      m_xUserInputKeyValue.push_back("v");
      m_xUserInputKeyCode.push_back(87);
      m_xUserInputKeyValue.push_back("w");
      m_xUserInputKeyCode.push_back(88);
      m_xUserInputKeyValue.push_back("x");
      m_xUserInputKeyCode.push_back(89);
      m_xUserInputKeyValue.push_back("y");
      m_xUserInputKeyCode.push_back(90);
      m_xUserInputKeyValue.push_back("z");
    }

    void TextUIUpdate(int _vx, int _vy)
    {
      if (m_iTextVX != _vx || m_iTextVY != _vy)
      {
        int old_vx = m_iTextVX;
        int old_vy = m_iTextVY;
        m_iTextVX = _vx;
        m_iTextVY = _vy;
        for (int i = 0; i<m_xMeshText.size(); i++)
        {
          aabb Old_aabb = m_xMeshText[i]->ReturnTextAABB();
          aabb New_aabb(vec3(Old_aabb.get_center().x()*m_iTextVX / old_vx, Old_aabb.get_center().y()*m_iTextVY / old_vy, Old_aabb.get_center().z()), vec3(Old_aabb.get_half_extent().x()*m_iTextVX / old_vx, Old_aabb.get_half_extent().y()*m_iTextVY / old_vy, 0.0f));
          m_xMeshText[i]->SetTextAABB(New_aabb);
        }
      }
      for (int i = 0; i<m_xMeshText.size(); i++)
      {

        m_xMeshText[i]->clear();
        switch (i)
        {
        case 0:m_xMeshText[i]->format("Mesh URL :\n%s", m_sAlgoString[i].c_str());
          break;
        case 1:m_xMeshText[i]->format("Target Num Faces :\n%s", m_sAlgoString[i].c_str());
          break;
        case 2:m_xMeshText[i]->format("Target Num Verticies :\n%s", m_sAlgoString[i].c_str());
          break;
        case 3:m_xMeshText[i]->format("Vertex Proxi Pair :\n%s", m_sAlgoString[i].c_str());
          break;
        case 4:m_xMeshText[i]->format("Max Error Threshold:\n%s", m_sAlgoString[i].c_str());
          break;
        case 5:m_xMeshText[i]->format("Contractions Per Run:\n%s", m_sAlgoString[i].c_str());
          break;
        }
        m_xMeshText[i]->update();
      }
      m_xoverlay->render(_vx, _vy);
    }

    void ConvertStringsToNumbers()
    {
      m_iTargetFaces = 0;
      for (int i = 0; i<m_sAlgoString[1].size(); i++)
      {
        m_iTargetFaces *= 10;
        if (m_sAlgoString[1][i] == '.' || m_sAlgoString[1][i] > 57 || m_sAlgoString[1][i] < 48)
        {
          break;
        }
        else
        {
          m_iTargetFaces += m_sAlgoString[1][i] - 48;
        }
      }

      m_iTargettri = 0;
      for (int i = 0; i<m_sAlgoString[2].size(); i++)
      {
        m_iTargettri *= 10;
        if (m_sAlgoString[2][i] == '.' || m_sAlgoString[2][i] > 57 || m_sAlgoString[2][i] < 48)
        {
          break;
        }
        else
        {
          m_iTargettri += m_sAlgoString[2][i] - 48;
        }
      }

      m_fVertProxi = 0.0f;
      bool bdotoc = false;
      float fdotdiv = 10.0f;
      for (int i = 0; i<m_sAlgoString[3].size(); i++)
      {
        if (!bdotoc && m_sAlgoString[3][i] != '.')
          m_fVertProxi *= 10;
        if (m_sAlgoString[3][i] > 57 || m_sAlgoString[3][i] < 48)
        {
          if (m_sAlgoString[3][i] == '.')
            bdotoc = true;
          else
            break;
        }
        else
        {
          if (!bdotoc)
          {
            m_fVertProxi += m_sAlgoString[3][i] - 48;
          }
          else
          {
            m_fVertProxi += (float)(m_sAlgoString[3][i] - 48) / fdotdiv;
            fdotdiv *= 10;
          }
        }
      }

      m_fMaxErrorth = 0.0f;
      bdotoc = false;
      fdotdiv = 10.0f;
      for (int i = 0; i<m_sAlgoString[4].size(); i++)
      {
        if (!bdotoc && m_sAlgoString[4][i] != '.')
          m_fMaxErrorth *= 10;
        if (m_sAlgoString[4][i] > 57 || m_sAlgoString[4][i] < 48)
        {
          if (m_sAlgoString[4][i] == '.')
            bdotoc = true;
          else
            break;
        }
        else
        {
          if (!bdotoc)
          {
            m_fMaxErrorth += m_sAlgoString[4][i] - 48;
          }
          else
          {
            m_fMaxErrorth += (float)(m_sAlgoString[4][i] - 48) / fdotdiv;
            fdotdiv *= 10;
          }
        }
      }

      m_iSimplificationPerRun = 0;
      for (int i = 0; i<m_sAlgoString[5].size(); i++)
      {
          m_iSimplificationPerRun *= 10;
        if (m_sAlgoString[5][i] == '.' || m_sAlgoString[5][i] > 57 || m_sAlgoString[5][i] < 48)
        {
          break;
        }
        else
        {
          m_iSimplificationPerRun += m_sAlgoString[5][i] - 48;
        }
      }
    }

    void KeyboardInputControl()
    {
      if (is_key_going_down(key::key_esc))
      {
        exit(0);
      }
      if (is_key_going_down(' '))
      {
          wireframe=!wireframe;
          if (wireframe)
          {
              glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
          }
          else
          {
              glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
          }
      }
      if (is_key_down(key::key_esc))
      {
        exit(1);
      }
      float speed = 1;
      if (is_key_down(key::key_shift))
      {
        speed = 0.08;
      }
      if (is_key_down('W'))
      {
        app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(0, speed, 0);
      }
      if (is_key_down('S'))
      {
        app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(0, -speed, 0);
      }
      if (is_key_down('E'))
      {
        app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(0, 0, -speed);
      }
      if (is_key_down('Q'))
      {
        app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(0, 0, speed);
      }
      if (is_key_down('A'))
      {
        app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(-speed, 0, 0);
      }
      if (is_key_down('D'))
      {
        app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(speed, 0, 0);
      }
      if (is_key_down('Z'))
      {
        app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().rotateX(-speed);
      }
      if (is_key_down('X'))
      {
        app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().rotateX(speed);
      }
      if (is_key_down('C'))
      {
        app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().rotateY(speed);
      }
      if (is_key_down('V'))
      {
        app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().rotateY(-speed);
      }
    }

    void SetVariables()
    {
        simples.SetContractionMaxCost(m_fMaxErrorth);
        simples.SetTargetNumVertices(m_iTargettri);
        simples.SetTargetNumTriangles(m_iTargetFaces);
        simples.SetRemovalsPerSimplification(m_iSimplificationPerRun);
        simples.SetPairThreshold(m_fVertProxi);
    }

  public:
    /// this is called when we construct the class before everything is initialised.
    Mesh_Simplification(int argc, char **argv) : app(argc, argv) {

        m_sMeshURL="newRex.obj";
        m_iTargetFaces=0;
        
        m_iTargettri=0;
        m_fVertProxi=0;
        m_fMaxErrorth=100000.0f;
        m_iSimplificationPerRun=500;
        wireframe =false;
    }

    ~Mesh_Simplification() {
      delete m_xoverlay;
      delete m_pxContinueButton;
      delete m_pxSubmitButton;
      delete m_pxSaveMeshButton;
      delete m_pxDeselect;
      for (int i = 0; i<m_xMeshText.size(); i++)
      {
        delete m_xMeshText[i];
      }
      for (int i = 0; i<m_pxUserInputFields.size(); i++)
      {
        delete m_pxUserInputFields[i];
      }
    }
    // texture for on select and deselect on field

    /// this is called once OpenGL is initialized
    void app_init() {
      app_scene = new visual_scene();
      app_scene->create_default_camera_and_lights();
      app_scene->get_camera_instance(0)->set_far_plane(1000);
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      FillInputKeyArray();
      SetVariables();
       importer = new Obj_Importer();
       mesh* importedMesh = new mesh;
      if (importer->loadObj(m_sMeshURL.c_str(),importedMesh))
      {
          
        importer->buildMesh();
        simples.Init(importedMesh);
        scene_node* node = new scene_node();
        node->scale(vec3(0.1f, 0.1f, 0.1f));
        app_scene->add_scene_node(node);

        app_scene->add_mesh_instance(new mesh_instance(node, importedMesh, new material(vec4(0, 1, 0, 1))));
      }
      
      //UI Buttons
      m_pxSubmitButton = new UI_Algo_start_Button(vec2(vx / 2 - 150, vy - 50), vec2(50, 35), -1.0f, vec2(0.0f, 0.7470f), vec2(0.25f, 0.9335f));
      m_pxContinueButton = new UI_Algo_start_Button(vec2(vx / 2, vy - 50), vec2(50, 35), -1.0f, vec2(0.5039f, 0.8144f), vec2(0.7568f, 1.0f));
      m_pxSaveMeshButton = new UI_Algo_start_Button(vec2(vx / 2 + 150, vy - 50), vec2(50, 35), -1.0f, vec2(0.2548f, 0.7470f), vec2(0.5019f, 0.9335f));
      m_pxDeselect = new Mesh_Name_Field(vec2(0, 0), vec2(vx, vy), -1.1f, vec2(0.0f, 0.0f), vec2(0.25f, 0.25f));
      Mesh_Name_Field* xTextFieldMeshName = new Mesh_Name_Field(vec2(75, 75), vec2(150, 30), -1.0f, vec2(0.0f, 0.9355f), vec2(0.25f, 1.0f));
      Mesh_Name_Field* xTextFieldsTargetNumFaces = new Mesh_Name_Field(vec2(75, 150), vec2(150, 30), -1.0f, vec2(0.0f, 0.9355f), vec2(0.25f, 1.0f));
      Mesh_Name_Field* xTextFieldsTargetNumTri = new Mesh_Name_Field(vec2(75, 225), vec2(150, 30), -1.0f, vec2(0.0f, 0.9355f), vec2(0.25f, 1.0f));
      Mesh_Name_Field* xTextFieldVtxProxPair = new Mesh_Name_Field(vec2(75, 300), vec2(150, 30), -1.0f, vec2(0.0f, 0.9355f), vec2(0.25f, 1.0f));
      Mesh_Name_Field* xTextFieldMaxErrTh = new Mesh_Name_Field(vec2(75, 375), vec2(150, 30), -1.0f, vec2(0.0f, 0.9355f), vec2(0.25f, 1.0f));
      Mesh_Name_Field* xContinuePerRun = new Mesh_Name_Field(vec2(75, 450), vec2(150, 30), -1.0f, vec2(0.0f, 0.9355f), vec2(0.25f, 1.0f));
      //create  new name fields, add to the system then to the dynarray 
      GUI_sys.init(vx, vy, this, "assets/UI_Mesh_Simplification_Sprite_Sheet.gif", "assets/UI_Mesh_Simplification_Sprite_Sheet_Mask.gif");
      GUI_sys.add_element(m_pxSubmitButton);
      GUI_sys.add_element(m_pxContinueButton);
      GUI_sys.add_element(xTextFieldMeshName);
      GUI_sys.add_element(xTextFieldsTargetNumFaces);
      GUI_sys.add_element(xTextFieldsTargetNumTri);
      GUI_sys.add_element(xTextFieldVtxProxPair);
      GUI_sys.add_element(xTextFieldMaxErrTh);
      GUI_sys.add_element(xContinuePerRun);
      GUI_sys.add_element(m_pxSaveMeshButton);
      GUI_sys.add_element(m_pxDeselect);
      //the dynarray for the UI Textfields
      m_pxUserInputFields.push_back(xTextFieldMeshName);
      m_pxUserInputFields.push_back(xTextFieldsTargetNumFaces);
      m_pxUserInputFields.push_back(xTextFieldsTargetNumTri);
      m_pxUserInputFields.push_back(xTextFieldVtxProxPair);
      m_pxUserInputFields.push_back(xTextFieldMaxErrTh);
      m_pxUserInputFields.push_back(xContinuePerRun);
      //The Strings Inputs from the user 
      //push back the string in the dyne array
      string sMeshName="newRex.obj";
      string sTargetNumFaces="0";
      string sTargetNumTri="0";
      string sVtxProxPair="0";
      string sMaxErrTh="100000";
      string sContinuePerRun="500";
      m_sAlgoString.push_back(sMeshName);
      m_sAlgoString.push_back(sTargetNumFaces);
      m_sAlgoString.push_back(sTargetNumTri);
      m_sAlgoString.push_back(sVtxProxPair);
      m_sAlgoString.push_back(sMaxErrTh);
      m_sAlgoString.push_back(sContinuePerRun);

      //UI Text
      m_iTextVX = vx;
      m_iTextVY = vy;
      //UI Text Output
      m_xoverlay = new text_overlay();
      // get the defualt font.
      bitmap_font *font = m_xoverlay->get_default_font();
      //UI Text
      aabb bb_Text1(vec3(150 - vx / 2, vy / 2 - 90, 1.0f), vec3(150.0f, 30.0f, 0.0f));
      mesh_text* UIText1 = new mesh_text(font, "Hi", &bb_Text1);

      aabb bb_Text2(vec3(150 - vx / 2, vy / 2 - 165, 1.0f), vec3(150.0f, 30.0f, 0.0f));
      mesh_text* UIText2 = new mesh_text(font, "", &bb_Text2);

      aabb bb_Text3(vec3(150 - vx / 2, vy / 2 - 240, 1.0f), vec3(150.0f, 30.0f, 0.0f));
      mesh_text* UIText3 = new mesh_text(font, "", &bb_Text3);

      aabb bb_Text4(vec3(150 - vx / 2, vy / 2 - 315, 1.0f), vec3(150.0f, 30.0f, 0.0f));
      mesh_text* UIText4 = new mesh_text(font, "", &bb_Text4);

      aabb bb_Text5(vec3(150 - vx / 2, vy / 2 - 390, 1.0f), vec3(150.0f, 30.0f, 0.0f));
      mesh_text* UIText5 = new mesh_text(font, "", &bb_Text5);

      aabb bb_Text6(vec3(150 - vx / 2, vy / 2 - 465, 1.0f), vec3(150.0f, 30.0f, 0.0f));
      mesh_text* UIText6 = new mesh_text(font, "", &bb_Text6);

      m_xoverlay->add_mesh_text(UIText1);
      m_xoverlay->add_mesh_text(UIText2);
      m_xoverlay->add_mesh_text(UIText3);
      m_xoverlay->add_mesh_text(UIText4);
      m_xoverlay->add_mesh_text(UIText5);
      m_xoverlay->add_mesh_text(UIText6);

      m_xMeshText.push_back(UIText1);
      m_xMeshText.push_back(UIText2);
      m_xMeshText.push_back(UIText3);
      m_xMeshText.push_back(UIText4);
      m_xMeshText.push_back(UIText5);
      m_xMeshText.push_back(UIText6);
    }

    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);

      // update matrices. assume 30 fps.
      KeyboardInputControl();
      app_scene->update(1.0f / 30);
      if (!m_bMeshNameOnFocus)
      {
        for (m_iFieldNumber = 0; m_iFieldNumber<m_pxUserInputFields.size(); m_iFieldNumber++)
        {
          m_bMeshNameOnFocus = m_pxUserInputFields[m_iFieldNumber]->IsMeshStringOnFocus();
          if (m_bMeshNameOnFocus)
          {
            m_pxUserInputFields[m_iFieldNumber]->Set_texture_UV(vec2(0.26f, 0.9355f), vec2(0.51f, 1.0f));
            m_pxDeselect->SetMeshStringOnFocus(false);
            break;
          }
        }
      }
      if (m_bMeshNameOnFocus)
      {
        m_bDeselOnFocus = m_pxDeselect->IsMeshStringOnFocus();
        if (m_bDeselOnFocus)
        {
          m_bMeshNameOnFocus = false;
          m_pxUserInputFields[m_iFieldNumber]->SetMeshStringOnFocus(false);
          m_pxUserInputFields[m_iFieldNumber]->Set_texture_UV(vec2(0.0f, 0.9355f), vec2(0.25f, 1.0f));
          m_pxDeselect->SetMeshStringOnFocus(false);
        }
        for (int j = 0; j<m_pxUserInputFields.size(); j++)
        {
          if (j != m_iFieldNumber)
          {
            if (m_pxUserInputFields[j]->IsMeshStringOnFocus())
            {
              m_pxUserInputFields[m_iFieldNumber]->SetMeshStringOnFocus(false); // change texture to normal
              m_pxUserInputFields[m_iFieldNumber]->Set_texture_UV(vec2(0.0f, 0.9355f), vec2(0.25f, 1.0f));
              m_iFieldNumber = j;
              m_pxUserInputFields[m_iFieldNumber]->Set_texture_UV(vec2(0.26f, 0.9355f), vec2(0.51f, 1.0f));
              m_pxDeselect->SetMeshStringOnFocus(false);
              break;
            }
          }
        }
        for (int i = 0; i<m_xUserInputKeyValue.size(); i++)//fill the size of dynarray
        {
          if (is_key_going_down(m_xUserInputKeyCode[i]))//check for all the a-z and number . and enter see for A-Z make a dynarray for codes
          {
            if (i != 0 && i != 1)//enter,backspace not pressed
            {
              m_sAlgoString[m_iFieldNumber] += m_xUserInputKeyValue[i];
            }
            else
            {
              if (i == 0)//enter presed
              {
                m_bMeshNameOnFocus = false;
                m_pxUserInputFields[m_iFieldNumber]->SetMeshStringOnFocus(false); // change the texture of the field
                m_pxUserInputFields[m_iFieldNumber]->Set_texture_UV(vec2(0.0f, 0.9355f), vec2(0.25f, 1.0f));
              }
              if (i == 1)//backspace pressed
              {
                if (m_sAlgoString[m_iFieldNumber].size()>0)
                  m_sAlgoString[m_iFieldNumber][m_sAlgoString[m_iFieldNumber].size() - 1] = '\0';
              }
            }
            break;
          }
        }
      }
      m_bFieldSubmit = m_pxSubmitButton->IsSubmit();
      if (m_bFieldSubmit)
      {
        m_pxSubmitButton->Set_texture_UV(vec2(0.2548f, 0.5566f), vec2(0.5019f, 0.7470f));
        m_pxSubmitButton->SetSubmit(false);
        ConvertStringsToNumbers();
        SetVariables();
        if (importer->loadObj(m_sMeshURL, app_scene->get_mesh_instance(0)->get_mesh()))
        {
            importer->buildMesh();
            simples.Init(app_scene->get_mesh_instance(0)->get_mesh());
        }
        //start the algo
      }

      m_bContinue = m_pxContinueButton->IsSubmit();
      if (m_bContinue)
      {
        if (!m_bContinueColourToggle)
        {
          m_pxContinueButton->Set_texture_UV(vec2(0.75039f, 0.8144f), vec2(1.0f, 1.0f));
        }
        else
        {
          m_pxContinueButton->Set_texture_UV(vec2(0.5039f, 0.8144f), vec2(0.75039f, 1.0f));
        }
        m_bContinueColourToggle = !m_bContinueColourToggle;
        ConvertStringsToNumbers();
        SetVariables();
        //continue to the next step        
        simples.Simplify(app_scene->get_mesh_instance(0)->get_mesh());
        m_pxContinueButton->SetSubmit(false);
      }
      

      m_bSaveMesh = m_pxSaveMeshButton->IsSubmit();
      if (m_bSaveMesh)
      {
        m_pxSaveMeshButton->SetSubmit(false);
        m_pxSaveMeshButton->Set_texture_UV(vec2(0.0f, 0.5566f), vec2(0.25f, 0.7470f));
        
        FILE* NewFile = fopen("OptimizedMesh.obj", "w");
        app_scene->get_mesh_instance(0)->get_mesh()->DumpToOBJ(NewFile,"OptimizedMesh", "");
      }
      app_scene->render((float)vx / vy);
      GUI_sys.update(vx, vy);
      TextUIUpdate(vx, vy);
    }
  };
}
