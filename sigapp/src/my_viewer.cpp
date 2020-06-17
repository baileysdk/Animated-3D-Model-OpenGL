
# include "my_viewer.h"

# include <sigogl/ui_button.h>
# include <sigogl/ui_radio_button.h>
# include <sig/sn_primitive.h>
# include <sig/sn_transform.h>
# include <sig/sn_manipulator.h>

# include <sigogl/ws_run.h>

// .obj Model Dimensions (for positioning purposes)
/*
	// Head Dimensions
	
		X: 9.9199	Y: 13.9643	Z: 27.5759

	// Torso Dimensions
	
		X: 27.401	Y: 13.578	Z: 39.3887

	// Arm Dimensions
	
		X: 11.4373	Y: 13.2638	Z: 34.3034

	// Leg Dimensions
	
		X: 8.2601	Y: 15.5055	Z: 98.7657
*/

// Project 1 : Animated Scene Global Variables
float da = (float)GS_PI / 180;
float rot_h = 0; float rot_b = 0;
float rot_al = 0; float rot_ar = 0;
float rot_ll = 0; float rot_lr = 0;

GsVec movement = GsVec(0.0f, 0.0f, 0.0f);
float rot_inc = 2.0f;
float total_x = 0.0f; float total_y = 0.0f;

bool camera_fixed = true;

// Scaling Matrix for Arm
GsMat arm_scale;

MyViewer::MyViewer ( int x, int y, int w, int h, const char* l ) : WsViewer(x,y,w,h,l)
{
	_nbut=0;
	_animating=false;

	// Initialize Manipulators
	man_total = new SnManipulator;
	man_head = new SnManipulator;
	man_body = new SnManipulator;
	man_arm_l = new SnManipulator;
	man_arm_r = new SnManipulator;
	man_leg_l = new SnManipulator;
	man_leg_r = new SnManipulator;

	initialize_object_parts();

	build_ui ();
	build_scene ();
}

void MyViewer::initialize_object_parts() {
	// Project 1 : Initializing Controller Variables

	// Initialize Translations for initial position
	t_total.translation(total_x, total_y, 0.0f);
	t_head.translation(GsVec(-2.2, 0.0, 38.4823));
	t_body.translation(GsVec(0.0, 0.0, 0.0));
	t_arm_l.translation(GsVec(17.6751, 3.6319, 19.57585));
	t_arm_r.translation(GsVec(-23.5751, 3.6319, 19.57585));
	t_leg_l.translation(GsVec(8.0005, -16.476375, -44.0772));
	t_leg_r.translation(GsVec(-9.2005, -16.876375, -44.0772));

	// Set Scale Adjustment for Arm Models (originals didn't have correct size compared to rest)
	arm_scale.scaling(1.2f);

	// Initialize the Rotation of the Body
	X_body.rotz(da * rot_b);
}

void MyViewer::build_ui ()
{
	UiPanel *p, *sp;
	UiManager* uim = WsWindow::uim();
	p = uim->add_panel ( "", UiPanel::HorizLeft );
	p->add ( new UiButton ( "View", sp=new UiPanel() ) );
	{	UiPanel* p=sp;
		p->add ( _nbut=new UiCheckButton ( "Normals", EvNormals ) ); 
	}
	p->add ( new UiButton ( "Animate", EvAnimate ) );
	p->add ( new UiButton ( "Exit", EvExit ) ); p->top()->separate();
}

void MyViewer::add_model ( SnShape* s, GsVec p )
{
	// This method demonstrates how to add some elements to our scene graph: lines,
	// and a shape, and all in a group under a SnManipulator.
	// Therefore we are also demonstrating the use of a manipulator to allow the user to
	// change the position of the object with the mouse. If you do not need mouse interaction,
	// you can just use a SnTransform to apply a transformation instead of a SnManipulator.
	// You would then add the transform as 1st element of the group, and set g->separator(true).
	// Your scene graph should always be carefully designed according to your application needs.

	SnManipulator* manip = new SnManipulator;
	GsMat m;
	m.translation ( p );
	manip->initial_mat ( m );

	SnGroup* g = new SnGroup;
	SnLines* l = new SnLines;
	l->color(GsColor::orange);
	g->add(s);
	g->add(l);
	manip->child(g);
	manip->visible(false); // call this to turn off mouse interaction

	rootg()->add(manip);
}

void MyViewer::build_scene ()
{
	// Head Object
	GsModel* head = new GsModel;
	head->load("models/head.obj");
	SnModel* h = new SnModel(head);
	SnGroup* sn_h = new SnGroup;
	h->color(GsColor::red);
	man_head->visible(false);
	man_head->initial_mat(t_head);
	sn_h->add(h);
	man_head->child(sn_h);


	// Left Arm Object 
	GsModel* arm_l = new GsModel;
	arm_l->load("models/arm_l.obj");
	SnModel* al = new SnModel(arm_l);
	SnGroup* sn_al = new SnGroup;
	al->color(GsColor::black);
	man_arm_l->visible(false);
	man_arm_l->initial_mat(t_arm_l * arm_scale);
	sn_al->add(al);
	man_arm_l->child(sn_al);


	// Right Arm Object
	GsModel* arm_r = new GsModel;
	arm_r->load("models/arm_r.obj");
	SnModel* ar = new SnModel(arm_r);
	SnGroup* sn_ar = new SnGroup;
	ar->color(GsColor::black);
	man_arm_r->visible(false);
	man_arm_r->initial_mat(t_arm_r * arm_scale);
	sn_ar->add(ar);
	man_arm_r->child(sn_ar);


	// Left Leg Object
	GsModel* leg_l = new GsModel;
	leg_l->load("models/leg_l.obj");
	SnModel* ll = new SnModel(leg_l);
	SnGroup* sn_ll = new SnGroup;
	ll->color(GsColor::black);
	man_leg_l->visible(false);
	man_leg_l->initial_mat(t_leg_l);
	sn_ll->add(ll);
	man_leg_l->child(sn_ll);


	// Right Leg Object
	GsModel* leg_r = new GsModel;
	leg_r->load("models/leg_r.obj");
	SnModel* lr = new SnModel(leg_r);
	SnGroup* sn_lr = new SnGroup;
	lr->color(GsColor::black);
	man_leg_r->visible(false);
	man_leg_r->initial_mat(t_leg_r);
	sn_lr->add(lr);
	man_leg_r->child(sn_lr);


	// Create a Group to control global movement
	// And add the individual parts to it
	SnGroup* sn_total_body = new SnGroup; 
	man_total->initial_mat(t_total);
	sn_total_body->add(man_body);
	man_total->child(sn_total_body);


	// Body Object (Torso)
	GsModel* body = new GsModel;
	body->load("models/torso.obj");
	SnModel* b = new SnModel(body);
	SnGroup* sn_b = new SnGroup;
	b->color(GsColor::black);
	man_body->visible(false);
	man_body->initial_mat(t_body);
	sn_b->add(b);
	sn_b->add(man_head);
	sn_b->add(man_arm_l);
	sn_b->add(man_arm_r);
	sn_b->add(man_leg_l);
	sn_b->add(man_leg_r);
	man_body->child(sn_b);

	// Create Floor and other Objects
	
	// Floor
	SnPrimitive* floor;
	floor = new SnPrimitive(GsPrimitive::Box, 100, 500, 1);
	floor->prim().material.diffuse = GsColor::darkgreen;
	add_model(floor, GsVec(0,0,-125));
	// Football
	SnPrimitive* football;
	football = new SnPrimitive(GsPrimitive::Ellipsoid, 15, .6f, 50);
	football->prim().material.diffuse = GsColor::brown;
	add_model(football, GsVec(50, 50, -115));
	// Ball w/ Texture
	GsModel* x;
	x = new GsModel;
	GsPnt center(-50.0f, -50.0f, -110.0f);
	x->make_sphere(center, 15, 50, true);
	SnModel* ball;
	ball = new SnModel(x);
	ball->color(GsColor(207, 72, 19));
	GsModel::Group& g = *x->G.push();
	g.fi = 0; g.fn = x->F.size();
	g.dmap = new GsModel::Texture;
	g.dmap->fname.set("models/marble.png");
	x->M.push().init();
	x->M.top() = GsMaterial();
	// Texture Coordinate Calculation
	int n = x->V.size();
	x->T.size(n);
	for (int i = 0; i < n; i++) {
		GsVec d = x->V[i] - center;
		d = d / 15;
		double u = 0.5 + ( atan2(d.z, d.x) / GS_2PI );
		double v = 0.5 - ( asin(d.y) / GS_PI );
		x->T[i].set(u, v);
	}
	x->set_mode(GsModel::Smooth, GsModel::PerGroupMtl);
	x->textured = true;
	add_model(ball, GsVec(0.0, 0.0, 0.0));


	// Push our Total Control Group to rootg
	rootg()->add(sn_total_body);
}

void MyViewer::camera_control() {
	// Rotation Matrix to control camera motion
	GsMat cam;
	cam.roty( da / 2 );

	if (camera_fixed) { // Reset Camera to a Fixed Position
		camera().eye = (GsPnt(0.0, 0.0, 1.0));
		camera().center = (GsPnt(0.0, 200.0, 0.0));
		camera().up = (GsPnt(0.0, 1.0, 0.0));
		camera().translate(GsVec(0.0, -400.0, 0.0));
		render();
		ws_check();
	}
	else { // Moving Camera to Pan around the Scene
		double lt, t0 = gs_time();
		do {
			lt = gs_time() - t0;
			camera().translate(GsVec(0.4, 0.8, 0.0));
			camera().rotate(cam);
			render();
			ws_check();
			message().setf("localtime = % f", lt);
		} while (lt < 5.0f);
	}
}

void MyViewer::fb_movement_logic(bool forward) {
	// Check Logic based on Forward or Backward Movement
	// Rotate the arms and Legs in a Meaningful Way based on Direction
	if (rot_lr >= 30 || rot_lr <= -30) { rot_inc *= -1; }
	if (forward) {
		movement.y -= 3.0f;
		rot_lr += rot_inc; rot_ll -= rot_inc;
		rot_ar -= rot_inc; rot_al += rot_inc;
	}
	else {
		movement.y += 3.0f;
		rot_lr -= rot_inc; rot_ll += rot_inc;
		rot_ar += rot_inc; rot_al -= rot_inc;
	}
	// Arm Rotation control
	X_arm_l.rotx(da * rot_al); man_arm_l->initial_mat(t_arm_l * X_arm_l * arm_scale);
	X_arm_r.rotx(da * rot_ar); man_arm_r->initial_mat(t_arm_r * X_arm_r * arm_scale);
	// Leg Rotation control
	X_leg_l.rotx(da * rot_ll); man_leg_l->initial_mat(t_leg_l * X_leg_l);
	X_leg_r.rotx(da * rot_lr); man_leg_r->initial_mat(t_leg_r * X_leg_r);
	// Body Movement control
	t_body.translation(movement); man_body->initial_mat(t_body * X_body);
}

void MyViewer::rl_movement_logic(bool left) {
	// Check Logic based on Left or Right rotation
	if (rot_b == 360 || rot_b == -360) { rot_b = 0; }
	if (left) { rot_b += 2; }
	else { rot_b -= 2; }
	// Body Rotation control
	X_body.rotz(da * rot_b); man_body->initial_mat(t_body * X_body);
}

void MyViewer::indiviual_part_logic(GsString part) {
	// Control the logic of the Head rotating
	if (part == "head_left" || part == "head_right") {  
		if (part == "head_left") { if (rot_h <= 30) { rot_h += 2; } }
		else { if (rot_h >= -30) { rot_h -= 2; } }
		Y_head.roty(da * rot_h); man_head->initial_mat(t_head * Y_head);
	}
	// Control the logic of the Left Arm rotating
	else if (part == "l_arm_forward" || part == "l_arm_backward") { 
		if (part == "l_arm_forward") { if (rot_al >= -140) { rot_al -= 2; } }
		else { if (rot_al <= 50) { rot_al += 2; } }
		X_arm_l.rotx(da * rot_al); man_arm_l->initial_mat(t_arm_l * X_arm_l * arm_scale);
	}
	// Control the logic of the Right Arm rotating
	else if (part == "r_arm_forward" || part == "r_arm_backward") { 
		if (part == "r_arm_forward") { if (rot_ar >= -140) { rot_ar -= 2; } }
		else { if (rot_ar <= 50) { rot_ar += 2; } }
		X_arm_r.rotx(da * rot_ar); man_arm_r->initial_mat(t_arm_r * X_arm_r * arm_scale);
	}
	// Control the logic of the Left Leg rotating
	else if (part == "l_leg_forward" || part == "l_leg_backward") { 
		if (part == "l_leg_forward") { if (rot_ll >= -30) { rot_ll -= 2; } }
		else { if (rot_ll <= 30) { rot_ll += 2; } }
		X_leg_l.rotx(da * rot_ll); man_leg_l->initial_mat(t_leg_l * X_leg_l);
	}
	// Control the logic of the Right Leg rotating
	else if (part == "r_leg_forward" || part == "r_leg_backward") { 
		if (part == "r_leg_forward") { if (rot_lr >= -30) { rot_lr -= 2; } } 
		else { if (rot_lr <= 30) { rot_lr += 2; } }
		X_leg_r.rotx(da * rot_lr); man_leg_r->initial_mat(t_leg_r * X_leg_r);
	}
}

// Below is an example of how to control the main loop of an animation:
void MyViewer::run_animation ()
{
	if ( _animating ) return; // avoid recursive calls
	_animating = true;
	
	int ind = gs_random ( 0, rootg()->size()-1 ); // pick one child
	SnManipulator* manip = rootg()->get<SnManipulator>(ind); // access one of the manipulators
	GsMat m = manip->mat();

	double frdt = 1.0/30.0; // delta time to reach given number of frames per second
	double v = 4; // target velocity is 1 unit per second
	double t=0, lt=0, t0=gs_time();
	do // run for a while:
	{	while ( t-lt<frdt ) { ws_check(); t=gs_time()-t0; } // wait until it is time for next frame
		double yinc = (t-lt)*v;
		if ( t>2 ) yinc=-yinc; // after 2 secs: go down
		lt = t;
		m.e24 += (float)yinc;
		if ( m.e24<0 ) m.e24=0; // make sure it does not go below 0
		manip->initial_mat ( m );
		render(); // notify it needs redraw
		ws_check(); // redraw now
	}	while ( m.e24>0 );
	_animating = false;
}

void MyViewer::show_normals ( bool view )
{
	// Note that primitives are only converted to meshes in GsModel
	// at the first draw call.
	GsArray<GsVec> fn;
	SnGroup* r = (SnGroup*)root();
	for ( int k=0; k<r->size(); k++ )
	{	SnManipulator* manip = r->get<SnManipulator>(k);
		SnShape* s = manip->child<SnGroup>()->get<SnShape>(0);
		SnLines* l = manip->child<SnGroup>()->get<SnLines>(1);
		if ( !view ) { l->visible(false); continue; }
		l->visible ( true );
		if ( !l->empty() ) continue; // build only once
		l->init();
		if ( s->instance_name()==SnPrimitive::class_name )
		{	GsModel& m = *((SnModel*)s)->model();
			m.get_normals_per_face ( fn );
			const GsVec* n = fn.pt();
			float f = 0.33f;
			for ( int i=0; i<m.F.size(); i++ )
			{	const GsVec& a=m.V[m.F[i].a]; l->push ( a, a+(*n++)*f );
				const GsVec& b=m.V[m.F[i].b]; l->push ( b, b+(*n++)*f );
				const GsVec& c=m.V[m.F[i].c]; l->push ( c, c+(*n++)*f );
			}
		}  
	}
}

int MyViewer::handle_keyboard ( const GsEvent &e )
{
	int ret = WsViewer::handle_keyboard ( e ); // 1st let system check events
	if ( ret ) return ret;

	switch ( e.key )
	{	
		case GsEvent::KeyEsc : gs_exit(); return 1;
		case 'n' : { bool b=!_nbut->value(); _nbut->value(b); show_normals(b); return 1; }

		case 'q': { indiviual_part_logic("head_left"); redraw(); return 1; } // Head Rotate Left
		case 'a': { indiviual_part_logic("head_right"); redraw(); return 1; } // Head Rotate Right

		case 'w': { indiviual_part_logic("l_arm_forward"); redraw(); return 1; } // Left Arm Rotate Forward
		case 's': { indiviual_part_logic("l_arm_backward"); redraw(); return 1; } // Left Arm Rotate Backward

		case 'e': { indiviual_part_logic("r_arm_forward"); redraw(); return 1; } // Right Arm Rotate Forward
		case 'd': { indiviual_part_logic("r_arm_backward"); redraw(); return 1; } // Right Arm Rotate Backward

		case 'r': { indiviual_part_logic("l_leg_forward"); redraw(); return 1; } // Left Leg Rotate Forward
		case 'f': { indiviual_part_logic("l_leg_backward"); redraw(); return 1; } // Left Leg Rotate Backward

		case 't': { indiviual_part_logic("r_leg_forward"); redraw(); return 1; } // Right Leg Rotate Forward
		case 'g': { indiviual_part_logic("r_leg_backward"); redraw(); return 1; } // Right Leg Rotate Backward

		case 65362: { fb_movement_logic(true); redraw(); return 1; } // Move Body Forward
		case 65364: { fb_movement_logic(false); redraw(); return 1; } // Move Body Backward
		case 65361: { rl_movement_logic(true); redraw(); return 1; } // Turn Body Left
		case 65363: { rl_movement_logic(false); redraw(); return 1; } // Turn Body Right

		case ' ': { camera_control(); camera_fixed = !camera_fixed; return 1; } // Change Camera Mode (Front vs Rotating)
	}

	return 0;
}

int MyViewer::uievent ( int e )
{
	switch ( e )
	{	case EvNormals: show_normals(_nbut->value()); return 1;
		case EvAnimate: run_animation(); return 1;
		case EvExit: gs_exit();
	}
	return WsViewer::uievent(e);
}
