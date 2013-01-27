/* +---------------------------------------------------------------------------+
   |                 The Mobile Robot Programming Toolkit (MRPT)               |
   |                                                                           |
   |                          http://www.mrpt.org/                             |
   |                                                                           |
   | Copyright (c) 2005-2013, Individual contributors, see AUTHORS file        |
   | Copyright (c) 2005-2013, MAPIR group, University of Malaga                |
   | Copyright (c) 2012-2013, University of Almeria                            |
   | All rights reserved.                                                      |
   |                                                                           |
   | Redistribution and use in source and binary forms, with or without        |
   | modification, are permitted provided that the following conditions are    |
   | met:                                                                      |
   |    * Redistributions of source code must retain the above copyright       |
   |      notice, this list of conditions and the following disclaimer.        |
   |    * Redistributions in binary form must reproduce the above copyright    |
   |      notice, this list of conditions and the following disclaimer in the  |
   |      documentation and/or other materials provided with the distribution. |
   |    * Neither the name of the copyright holders nor the                    |
   |      names of its contributors may be used to endorse or promote products |
   |      derived from this software without specific prior written permission.|
   |                                                                           |
   | THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       |
   | 'AS IS' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED |
   | TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR|
   | PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE |
   | FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL|
   | DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR|
   |  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)       |
   | HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,       |
   | STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN  |
   | ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE           |
   | POSSIBILITY OF SUCH DAMAGE.                                               |
   +---------------------------------------------------------------------------+ */

#include <mrpt/srba.h>
#include <mrpt/gui.h>  // For rendering results as a 3D scene
#include <mrpt/random.h>  // For rendering results as a 3D scene

using namespace mrpt::srba;
using namespace mrpt::random;
using namespace std;

// --------------------------------------------------------------------------------
// Declare a typedef "my_srba_t" for easily referring to my RBA problem type:
// --------------------------------------------------------------------------------
struct my_srba_options
{
	typedef sensor_pose_on_robot_se3 sensor_pose_on_robot_t;
};

typedef RBA_Problem<
	kf2kf_poses::SE3,                // Parameterization  KF-to-KF poses
	landmarks::Euclidean3D,          // Parameterization of landmark positions    
	observations::MonocularCamera,    // Type of observations
	my_srba_options                  // Other parameters
	> 
	my_srba_t;

// --------------------------------------------------------------------------------
// A test dataset. Generated with http://code.google.com/p/recursive-world-toolkit/ 
//  and the script: tutorials_dataset-monocular.cfg
// --------------------------------------------------------------------------------
const double SENSOR_NOISE_STD = 1e-4;

struct basic_monocular_dataset_entry_t 
{
	unsigned int landmark_id;
	double px_x, px_y; 
};
// Observations for KF#0: GT_Pose= 0.000000 0.000000 0.000000 0.564787 -0.517532 0.434261 -0.473913
basic_monocular_dataset_entry_t dataset0[] = {
 {   146, 397.16046143, 371.29309082},
 {   144, 325.15927124, 432.81069946},
 {   118, 191.95759583, 391.00930786},
 {    55, 556.79144287, 405.27206421},
 {    39, 427.63507080, 337.36282349},
 {    35, 403.34268188, 345.97265625},
 {    20,  19.02102661, 376.73773193},
 {    28, 437.69879150, 350.98776245},
 {    52, 173.58599854, 203.18234253},
 {   141, 490.49987793, 384.93002319},
 {    84, 285.96942139, 291.06143188},
 {    76, 448.31323242, 298.45452881},
 {    71, 399.07693481, 195.52038574},
 {    37, 485.42114258, 248.38455200},
 {   100, 162.51091003, 170.75312805},
 {   127, 293.27337646, 409.78048706},
 {   115, 186.95903015, 264.30618286},
 {    59, 492.91369629, 300.95275879},
 {     6, 492.14477539, 370.06378174},
 {   113, 298.54818726, 336.68572998},
 {    99, 209.10960388, 364.52743530},
 {   128, 340.99240112, 268.45800781},
 {    86, 370.48016357, 251.53149414},
 {    13, 373.14675903, 373.82769775}
};
// Observations for KF#10: GT_Pose = 1.226072 0.293638 0.110099 0.521317 -0.478835 0.477946 -0.520108
basic_monocular_dataset_entry_t dataset1[] = {
 {   146, 394.29074097, 374.71395874},
 {   144, 318.19305420, 439.96682739},
 {   118, 173.66944885, 397.46075439},
 {    55, 559.21221924, 408.94757080},
 {    39, 425.57754517, 338.52114868},
 {    35, 400.46832275, 347.35559082},
 {    28, 435.63137817, 352.28668213},
 {   141, 489.60995483, 387.01168823},
 {    52, 156.23440552, 196.46017456},
 {    84, 278.39483643, 290.47094727},
 {    76, 446.40863037, 298.43890381},
 {    71, 396.24160767, 192.49893188},
 {    37, 484.15655518, 247.46858215},
 {   127, 286.04342651, 413.77871704},
 {   115, 172.68215942, 262.00924683},
 {   100, 145.32748413, 162.40222168},
 {    59, 491.41613770, 301.05285645},
 {     6, 490.51980591, 371.28356934},
 {   113, 292.10940552, 337.68374634},
 {    99, 197.11656189, 367.19949341},
 {   128, 336.37933350, 267.47598267},
 {    86, 366.76464844, 250.29171753},
 {    13, 369.32812500, 375.40686035},
 {    94, 250.25048828, 172.92965698},
};

int main(int argc, char**argv)
{
	my_srba_t rba;     //  Create an empty RBA problem

	// --------------------------------------------------------------------------------
	// Set parameters 
	// --------------------------------------------------------------------------------
	rba.setVerbosityLevel( 2 );   // 0: None; 1:Important only; 2:Verbose

	rba.parameters.srba.use_robust_kernel = true;
	rba.parameters.srba.std_noise_observations = 0.5;  // pixels

	// =========== Topology parameters ===========
	rba.parameters.srba.edge_creation_policy = mrpt::srba::ecpICRA2013;
	rba.parameters.srba.max_tree_depth       = 3;
	rba.parameters.srba.max_optimize_depth   = 3;
	// ===========================================

	// Set camera calib:
	mrpt::utils::TCamera & c = rba.parameters.sensor.camera_calib;
	c.ncols = 800;
	c.nrows = 640;
	c.cx(400);
	c.cy(320);
	c.fx(200);
	c.fy(200);
	c.dist.setZero();

	// Sensor pose on the robot parameters:
	rba.parameters.sensor_pose.relative_pose = mrpt::poses::CPose3D(0,0,0,DEG2RAD(-90),DEG2RAD(0),DEG2RAD(-90) ); // Set camera pointing forwards (camera's +Z is robot +X)
	
	// Alternatively, parameters can be loaded from an .ini-like config file
	// -----------------------------------------------------------------------
	// rba.parameters.loadFromConfigFileName("config_file.cfg", "srba");
	//rba.sensor_params.camera_calib.loadFromConfigFile("CAMERA","config_file.cfg");
	
	// --------------------------------------------------------------------------------
	// Dump parameters to console (for checking/debugging only)
	// --------------------------------------------------------------------------------
	//cout << "RBA parameters:\n-----------------\n";
	//rba.parameters.dumpToConsole();

	// --------------------------------------------------------------------------------
	// Define observations of KF #0:
	// --------------------------------------------------------------------------------
	my_srba_t::new_kf_observations_t  list_obs;
	my_srba_t::new_kf_observation_t   obs_field;

	obs_field.is_fixed = false;   // Landmarks have unknown relative positions (i.e. treat them as unknowns to be estimated)
	obs_field.is_unknown_with_init_val = false; // We don't have any guess on the initial LM position (will invoke the inverse sensor model)

	for (size_t i=0;i<sizeof(dataset0)/sizeof(dataset0[0]);i++)
	{
		obs_field.obs.feat_id = dataset0[i].landmark_id;
		obs_field.obs.obs_data.px.x = dataset0[i].px_x + randomGenerator.drawGaussian1D(0,SENSOR_NOISE_STD);
		obs_field.obs.obs_data.px.y = dataset0[i].px_y + randomGenerator.drawGaussian1D(0,SENSOR_NOISE_STD);
		list_obs.push_back( obs_field );
	}


	//  Here happens the main stuff: create Key-frames, build structures, run optimization, etc.
	//  ============================================================================================
	srba::TNewKeyFrameInfo new_kf_info;
	rba.define_new_keyframe(
		list_obs,      // Input observations for the new KF
		new_kf_info,   // Output info
		true           // Also run local optimization?
		);

	cout << "Created KF #" << new_kf_info.kf_id 
		<< " | # kf-to-kf edges created:" <<  new_kf_info.created_edge_ids.size()  << endl
		<< "Optimization error: " << new_kf_info.optimize_results.total_sqr_error_init << " -> " << new_kf_info.optimize_results.total_sqr_error_final << endl
		<< "-------------------------------------------------------" << endl;


	// --------------------------------------------------------------------------------
	// Define observations of KF #1:
	// --------------------------------------------------------------------------------
	list_obs.clear();

	for (size_t i=0;i<sizeof(dataset1)/sizeof(dataset1[0]);i++)
	{
		obs_field.obs.feat_id = dataset1[i].landmark_id;
		obs_field.obs.obs_data.px.x = dataset1[i].px_x + randomGenerator.drawGaussian1D(0,SENSOR_NOISE_STD);
		obs_field.obs.obs_data.px.y = dataset1[i].px_y + randomGenerator.drawGaussian1D(0,SENSOR_NOISE_STD);
		list_obs.push_back( obs_field );
	}
	
	//  Here happens the main stuff: create Key-frames, build structures, run optimization, etc.
	//  ============================================================================================
	rba.define_new_keyframe(
		list_obs,      // Input observations for the new KF
		new_kf_info,   // Output info
		true           // Also run local optimization?
		);

	cout << "Created KF #" << new_kf_info.kf_id 
		<< " | # kf-to-kf edges created:" <<  new_kf_info.created_edge_ids.size() << endl
		<< "Optimization error: " << new_kf_info.optimize_results.total_sqr_error_init << " -> " << new_kf_info.optimize_results.total_sqr_error_final << endl
		<< "-------------------------------------------------------" << endl;


	// Dump the relative pose of KF#0 wrt KF#1:
	cout << "inv_pose of KF-to-KF edge #0 (relative pose of KF#0 wrt KF#1):\n" << rba.get_k2k_edges()[0].inv_pose << endl;

	// --------------------------------------------------------------------------------
	// Saving RBA graph as a DOT file:
	// --------------------------------------------------------------------------------
	const string sFil = "graph.dot";
	cout << "Saving final graph of KFs and LMs to: " << sFil << endl;
	rba.save_graph_as_dot(sFil, true /* LMs=save */);
	cout << "Done.\n";

	// --------------------------------------------------------------------------------
	// Show 3D view of the resulting map:
	// --------------------------------------------------------------------------------
	my_srba_t::TOpenGLRepresentationOptions  opengl_options;
	mrpt::opengl::CSetOfObjectsPtr rba_3d = mrpt::opengl::CSetOfObjects::Create();

	rba.build_opengl_representation(
		0,  // Root KF,
		opengl_options, // Rendering options
		rba_3d  // Output scene 
		);

	// Display:
#if MRPT_HAS_WXWIDGETS
	mrpt::gui::CDisplayWindow3D win("RBA results",640,480);
	{
		mrpt::opengl::COpenGLScenePtr &scene = win.get3DSceneAndLock();
		scene->insert(rba_3d);
		win.unlockAccess3DScene();
	}
	win.setCameraZoom( 4 ); 
	win.repaint();

	cout << "Press any key or close window to exit.\n";
	win.waitForKey();
#endif
		
	return 0; // All ok
}
