# pragma once

# include <sig/sn_poly_editor.h>
# include <sig/sn_lines2.h>
# include <sig/sn_manipulator.h>

# include <sigogl/ui_button.h>
# include <sigogl/ws_viewer.h>

// Viewer for this example:
class MyViewer : public WsViewer
{  protected :
	enum MenuEv { EvNormals, EvAnimate, EvExit };
	UiCheckButton* _nbut;
	bool _animating;

	// Project 1 : Class Variables to Control Hierarchical Object

	SnManipulator* man_total;
	SnManipulator* man_head; 
	SnManipulator* man_body; 
	SnManipulator* man_arm_l;
	SnManipulator* man_arm_r;
	SnManipulator* man_leg_l; 
	SnManipulator* man_leg_r;

	// Translation Matrices for Cartesian movement
	GsMat t_total;
	GsMat t_head; 
	GsMat t_body; 
	GsMat t_arm_l; 
	GsMat t_arm_r; 
	GsMat t_leg_l; 
	GsMat t_leg_r;

	// Rotation matrices for Individual part rotation and total body rotation
	GsMat Y_head;
	GsMat X_body; 
	GsMat X_arm_l; 
	GsMat X_arm_r; 
	GsMat X_leg_l; 
	GsMat X_leg_r;

   public :
	MyViewer ( int x, int y, int w, int h, const char* l );
	void initialize_object_parts();
	void build_ui ();
	void add_model ( SnShape* s, GsVec p );
	void build_scene ();
	void camera_control();
	void fb_movement_logic(bool forward);
	void rl_movement_logic(bool left);
	void indiviual_part_logic(GsString part);
	void show_normals ( bool view );
	void run_animation ();
	virtual int handle_keyboard ( const GsEvent &e ) override;
	virtual int uievent ( int e ) override;
};

