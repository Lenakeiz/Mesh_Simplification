////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
namespace octet {
  /// Scene containing a box with octet.

#include "EQMeshSimplifier.h"

  class Mesh_Simplification : public app {
    // scene for drawing box
    ref<visual_scene> app_scene;
    UI_sys GUI_sys;
    UI_Algo_start_Button* m_xSubmitButton;

	ref<mesh> importedMesh;
    string m_sMeshURL;
    int m_iTargetFaces;
    int m_iTargettri;
    float m_fVertProxi;
    float m_fMaxErrorth;

    int m_iFieldNumber;
    bool m_bMeshNameOnFocus = false;
    bool m_bFieldSubmit = false;
    
    dynarray<string> m_sAlgoString;
    dynarray<Mesh_Name_Field*> m_pxUserInputFields;
    dynarray<int> m_xUserInputKeyCode;
    dynarray<string> m_xUserInputKeyValue;
    
    text_overlay *m_xoverlay;
    dynarray<mesh_text*> m_xMeshText;
    int m_iTextVX,m_iTextVY;

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

    void TextUIUpdate(int _vx,int _vy)
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
      for (int i = 0; i<m_xMeshText.size();i++)
      {
      
      m_xMeshText[i]->clear();
      switch (i)
      {
        case 0:m_xMeshText[i]->format("Mesh URL :\n%s", m_sAlgoString[i].c_str());
               break;
        case 1:m_xMeshText[i]->format("Target Num Faces :\n%s", m_sAlgoString[i].c_str());
               break;
        case 2:m_xMeshText[i]->format("Target Num Tri :\n%s", m_sAlgoString[i].c_str());
          break;
        case 3:m_xMeshText[i]->format("Vertex Proxi Pair :\n%s", m_sAlgoString[i].c_str());
          break;
        case 4:m_xMeshText[i]->format("Max Error Threshold:\n%s", m_sAlgoString[i].c_str());
          break;
      }
       m_xMeshText[i]->update();
      }
      m_xoverlay->render(_vx,_vy);
    }

    void ConvertStringsToNumbers()
    {
      m_iTargetFaces=0;
      for (int i = 0; i<m_sAlgoString[1].size();i++)
      {
        m_iTargetFaces*=10;
        if (m_sAlgoString[1][i] == '.' || m_sAlgoString[1][i] > 57 || m_sAlgoString[1][i] < 48)
        {
          break;
        }
        else
        {
          m_iTargetFaces += m_sAlgoString[1][i]-48;
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
        if ( m_sAlgoString[3][i] > 57 || m_sAlgoString[3][i] < 48)
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
              fdotdiv*=10;
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

    }

	void KeyboardInputControl()
	{
		if (is_key_going_down(key::key_esc))
		{
			exit(0);
		}
		if (is_key_going_down('1'))
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else if (is_key_going_down('2'))
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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

  public:
    /// this is called when we construct the class before everything is initialised.
    Mesh_Simplification(int argc, char **argv) : app(argc, argv) {
    }

    ~Mesh_Simplification() {
      delete m_xoverlay;
      for (int i = 0; i<m_xMeshText.size();i++)
      {
        delete m_xMeshText[i];
      }
      for (int i = 0; i<m_pxUserInputFields.size(); i++)
      {
        delete m_pxUserInputFields[i];
      }      
    }

    /// this is called once OpenGL is initialized
    void app_init() {
      app_scene =  new visual_scene();
      app_scene->create_default_camera_and_lights();
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      FillInputKeyArray();

	  Obj_Importer* importer = new Obj_Importer("NewRex.obj");
	  if (importer->loadObj())
	  {
		  importedMesh = importer->buildMesh();
		  scene_node* node = new scene_node();
		  node->scale(vec3(0.1f, 0.1f, 0.1f));
		  app_scene->add_scene_node(node);

		  app_scene->add_mesh_instance(new mesh_instance(node, importedMesh, new material(vec4(0, 1, 0, 1))));
	  }

      //UI Buttons
      m_xSubmitButton = new UI_Algo_start_Button(vec2(vx/2, vy-50), vec2(50,35), -1.0f, vec2(), vec2());
      Mesh_Name_Field* xTextFieldMeshName = new Mesh_Name_Field(vec2(75, 75), vec2(150, 30), -1.0f, vec2(), vec2());
      Mesh_Name_Field* xTextFieldsTargetNumFaces = new Mesh_Name_Field(vec2(75, 150), vec2(150, 30), -1.0f, vec2(), vec2());
      Mesh_Name_Field* xTextFieldsTargetNumTri = new Mesh_Name_Field(vec2(75, 225), vec2(150, 30), -1.0f, vec2(), vec2());
      Mesh_Name_Field* xTextFieldVtxProxPair = new Mesh_Name_Field(vec2(75, 300), vec2(150, 30), -1.0f, vec2(), vec2());
      Mesh_Name_Field* xTextFieldMaxErrTh = new Mesh_Name_Field(vec2(75, 375), vec2(150, 30), -1.0f, vec2(), vec2());
      
      //create  new name fields, add to the system then to the dynarray 
      GUI_sys.init(vx, vy, this);   
      GUI_sys.add_element(m_xSubmitButton);
      GUI_sys.add_element(xTextFieldMeshName);
      GUI_sys.add_element(xTextFieldsTargetNumFaces);
      GUI_sys.add_element(xTextFieldsTargetNumTri);
      GUI_sys.add_element(xTextFieldVtxProxPair);
      GUI_sys.add_element(xTextFieldMaxErrTh);
      //the dynarray for the UI Textfields
      m_pxUserInputFields.push_back(xTextFieldMeshName);
      m_pxUserInputFields.push_back(xTextFieldsTargetNumFaces);
      m_pxUserInputFields.push_back(xTextFieldsTargetNumTri);
      m_pxUserInputFields.push_back(xTextFieldVtxProxPair);
      m_pxUserInputFields.push_back(xTextFieldMaxErrTh);
      
      //The Strings Inputs from the user 
      //push back the string in the dyne array
      string sMeshName;
      string sTargetNumFaces;
      string sTargetNumTri;
      string sVtxProxPair;
      string sMaxErrTh;
      m_sAlgoString.push_back(sMeshName);
      m_sAlgoString.push_back(sTargetNumFaces);
      m_sAlgoString.push_back(sTargetNumTri);
      m_sAlgoString.push_back(sVtxProxPair);
      m_sAlgoString.push_back(sMaxErrTh);
      
      //UI Text
      m_iTextVX = vx;
      m_iTextVY = vy;
      //UI Text Output
      m_xoverlay = new text_overlay();
      // get the defualt font.
      bitmap_font *font = m_xoverlay->get_default_font();
      //UI Text
      aabb bb_Text1(vec3(150 - vx / 2, vy / 2 - 90, 1.0f), vec3(150.0f, 30.0f, 0.0f));
      mesh_text* UIText1 = new mesh_text(font, "", &bb_Text1);
      
      aabb bb_Text2(vec3(150 - vx / 2, vy / 2 - 165, 1.0f), vec3(150.0f, 30.0f, 0.0f));
      mesh_text* UIText2 = new mesh_text(font,"",&bb_Text2);

      aabb bb_Text3(vec3(150 - vx / 2, vy / 2 - 240, 1.0f), vec3(150.0f, 30.0f, 0.0f));
      mesh_text* UIText3 = new mesh_text(font, "", &bb_Text3);

      aabb bb_Text4(vec3(150 - vx / 2, vy / 2 - 315, 1.0f), vec3(150.0f, 30.0f, 0.0f));
      mesh_text* UIText4 = new mesh_text(font, "", &bb_Text4);

      aabb bb_Text5(vec3(150 - vx / 2, vy / 2 - 390, 1.0f), vec3(150.0f, 30.0f, 0.0f));
      mesh_text* UIText5 = new mesh_text(font, "", &bb_Text5);
      m_xoverlay->add_mesh_text(UIText1);
      m_xoverlay->add_mesh_text(UIText2);
      m_xoverlay->add_mesh_text(UIText3);
      m_xoverlay->add_mesh_text(UIText4);
      m_xoverlay->add_mesh_text(UIText5);
    
      m_xMeshText.push_back(UIText1);
      m_xMeshText.push_back(UIText2);
      m_xMeshText.push_back(UIText3);
      m_xMeshText.push_back(UIText4);
      m_xMeshText.push_back(UIText5);
    }

    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {

	  KeyboardInputControl();

      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy, vec4(0,0,0,1));

      // update matrices. assume 30 fps.
      app_scene->update(1.0f/30);
      if (!m_bMeshNameOnFocus)
      {
        for (m_iFieldNumber = 0; m_iFieldNumber<m_pxUserInputFields.size(); m_iFieldNumber++)
        {
          m_bMeshNameOnFocus = m_pxUserInputFields[m_iFieldNumber]->IsMeshStringOnFocus();
          if (m_bMeshNameOnFocus)
            { 
              break;
            }
        }
      }
      if (m_bMeshNameOnFocus)
      {
        for (int i = 0; i<m_xUserInputKeyValue.size(); i++)//fill the size of dynarray
        {
          if (is_key_going_down(m_xUserInputKeyCode[i]))//check for all the a-z and number . and enter see for A-Z make a dynarray for codes
          {
            if (i!=0 && i!= 1)//enter,backspace not pressed
            {
              m_sAlgoString[m_iFieldNumber]+=m_xUserInputKeyValue[i];
            }
            else
            {              
              if(i==0)//enter presed
              {
                m_bMeshNameOnFocus = false;              
                m_pxUserInputFields[m_iFieldNumber]->SetMeshStringOnFocus(false);
                
              }
              if (i==1)//backspace pressed
              {
                m_sAlgoString[m_iFieldNumber][m_sAlgoString[m_iFieldNumber].size()-1] = '\0';
              }
            }
            break;
          }
        }
      }     
      m_bFieldSubmit = m_xSubmitButton->IsSubmit();
      if (m_bFieldSubmit)
      {
        m_xSubmitButton->SetSubmit(false);
        ConvertStringsToNumbers();
        //start the algo
      }
      app_scene->render((float)vx / vy);
      GUI_sys.update(vx, vy);
      TextUIUpdate(vx,vy);
    }
  };
}
