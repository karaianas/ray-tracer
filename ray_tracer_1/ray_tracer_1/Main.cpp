////////////////////////////////////////
// Main.cpp
////////////////////////////////////////

#include "Core.h"

// Path tracing
#include "MeshObject.h"
#include "InstanceObject.h"
#include "Camera.h"
#include "PointLight.h"
#include "DirectLight.h"
#include "Scene.h"
#include "LambertMaterial.h"
#include "FresnelMetalMaterial.h"
#include "RayTrace.h"
#include "BoxTreeObject.h"
#include "Random.h"

// Image processing
#include "Img.h"

using namespace std;

Img* I;
Camera cam;
int height = 1024;// 960;
int width = 1024;//1280

void project1();
void project2();
void project3();

void filter_test();
void adaptiveRendering();

////////////////////////////////////////////////////////////////////////////////

int main(int argc,char **argv) 
{
	I = new Img(height, width);
	adaptiveRendering();

	while (1)
	{
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
void adaptiveRendering()
{
	// Create scene
	Scene scn;
	scn.SetSkyColor(Color(0.8f, 0.9f, 1.0f));

	// Create ground
	LambertMaterial groundMtl;
	groundMtl.SetColor(Color(0.25f, 0.25f, 0.25f));
	MeshObject ground;
	ground.MakeBox(2.0f, 0.11f, 2.0f, &groundMtl);
	scn.AddObject(ground);

	// Load dragon mesh
	MeshObject dragon;
	dragon.LoadPLY(".//Models//dragon.ply");

	// Create box tree
	BoxTreeObject tree;
	tree.Construct(dragon);

	// Materials
	LambertMaterial white;
	white.SetColor(Color(0.7f, 0.7f, 0.7f));
	LambertMaterial red;
	red.SetColor(Color(0.7f, 0.1f, 0.1f));
	FresnelMetalMaterial metal;
	metal.SetColor(Color(0.95f, 0.64f, 0.54f));
	const int numDragons = 4;
	Material *mtl[numDragons] = { &white,&metal,&red,&white };

	// Create dragon instances
	glm::mat4 mtx;
	for (int i = 0; i<numDragons; i++) 
	{
		InstanceObject *inst = new InstanceObject(tree);
		mtx[3] = glm::vec4(0.0f, 0.0f, 0.3f*(float(i) / float(numDragons - 1) - 0.5f), 1.0f);
		inst->SetMatrix(mtx);
		inst->SetMaterial(mtl[i]);
		scn.AddObject(*inst);
	}

	// Create lights
	DirectLight sunlgt;
	sunlgt.SetBaseColor(Color(1.0f, 1.0f, 0.9f));
	sunlgt.SetIntensity(1.0f);
	sunlgt.SetDirection(glm::vec3(2.0f, -3.0f, -2.0f));
	scn.AddLight(sunlgt);

	// Create camera
	cam.SetResolution(width, height);
	cam.SetAspect(float(width) / (float)height);
	cam.LookAt(1.3f *glm::vec3(-0.5f, 0.15f, -0.2f), glm::vec3(0.0f, 0.15f, 0.0f), glm::vec3(0, 1.0f, 0));
	cam.SetFOV(40.0f);

	// Camera settings
	int nx, ny;
	cout << "Enter the sample budget: ";
	cin >> nx >> ny;
	I->setBudget(nx * ny);
	cam.SetJitter(true);
	cam.I = I;

	clock_t startTime;
	startTime = clock();
	// Filter settings
	int niter = 6;
	for (int i = 0; i < niter; i++)
	{
		cout << "===================== " << i << " =====================" << endl;

		clock_t t;
		t = clock();

		// Render image
		cam.Render(scn);

		// (1) Filter variances
		I->setConstants(1, 3);
		I->setConstants2(4.0f, 1.0f, 0.45f * 0.45f);
		I->Filter(1);

		// (2) Filter images
		// (3, 1) works best for 4by4
		I->setConstants(1, 3);// (7, 3) in the original paper(1, 3)
		I->setConstants2(0.5f, 1.0f, 0.45f * 0.45f);
		I->Filter(0);

		// (3) Compute error
		I->computeError();

		t = clock() - t;
		float seconds = ((double)t) / CLOCKS_PER_SEC;
		printf("Iteration Time: %2.2f seconds\n", seconds);

		//// Save image
		//string name = "D://Github//temp//test_";
		//name += to_string(nx);
		//name += "by";
		//name += to_string(ny);
		//name += "_";
		//name += to_string(seconds);
		//name += ".bmp";
		//cam.SaveBitmap((char*)name.c_str());

		cout << endl;
	}

	clock_t dT = clock() - startTime;
	float duration = ((double)dT) / CLOCKS_PER_SEC;
	printf("Total Time: %2.2f seconds\n", duration);
}

void filter_test()
{
	for (int i = 0; i < 4; i++)
	{
		// (1) Filter variances
		I->setConstants(1, 3);// 1, 3
		I->setConstants2(4.0f, 1.0f, 0.45f * 0.45f);
		I->Filter(1);

		// (2) Filter images
		// (3, 1) works best for 4by4
		I->setConstants(3, 1);// (7, 3) in the original paper
		I->setConstants2(0.5f, 1.0f, 0.45f * 0.45f);
		I->Filter(0);

		// (3) Compute error
		I->computeError();
	}
}

void project3()
{
	// Create scene
	Scene scn;
	scn.SetSkyColor(Color(0.8f, 0.9f, 1.0f));

	// Create ground
	LambertMaterial groundMtl;
	groundMtl.SetColor(Color(0.25f, 0.25f, 0.25f));
	MeshObject ground;
	ground.MakeBox(2.0f, 0.11f, 2.0f, &groundMtl);
	scn.AddObject(ground);
	
	// Load dragon mesh
	MeshObject dragon;
	dragon.LoadPLY(".//Models//dragon.ply");
	
	// Create box tree
	BoxTreeObject tree;
	tree.Construct(dragon);
	
	// Materials
	LambertMaterial white;
	white.SetColor(Color(0.7f, 0.7f, 0.7f));
	LambertMaterial red;
	red.SetColor(Color(0.7f, 0.1f, 0.1f));
	FresnelMetalMaterial metal;
	metal.SetColor(Color(0.95f, 0.64f, 0.54f));
	const int numDragons = 4;
	Material *mtl[numDragons] = { &white,&metal,&red,&white };
	
	// Create dragon instances
	glm::mat4 mtx;
	for (int i = 0; i<numDragons; i++) {
		InstanceObject *inst = new InstanceObject(tree);
		mtx[3] = glm::vec4(0.0f, 0.0f, 0.3f*(float(i) / float(numDragons - 1) - 0.5f), 1.0f);
		inst->SetMatrix(mtx);
		inst->SetMaterial(mtl[i]);
		scn.AddObject(*inst);
	}
	
	// Create lights
	DirectLight sunlgt;
	sunlgt.SetBaseColor(Color(1.0f, 1.0f, 0.9f));
	sunlgt.SetIntensity(1.0f);
	sunlgt.SetDirection(glm::vec3(2.0f, -3.0f, -2.0f));
	scn.AddLight(sunlgt);
	
	// Create camera
	Camera cam; 
	cam.SetResolution(640, 480);
	cam.SetAspect(1.33f);
	cam.LookAt(glm::vec3(-0.5f, 0.25f, -0.2f), glm::vec3(0.0f, 0.15f, 0.0f), glm::vec3(0, 1.0f, 0));
	cam.SetFOV(40.0f);
	
	int nx = 8;
	int ny = 8;
	cam.SetSuperSample(nx, ny);
	cam.SetJitter(true);
	//cam.SetShirley(true);
	
	// Render image
	cam.Render(scn);

	// Save image
	string name = "D://Github//temp//test_";
	name += to_string(nx);
	name += "by";
	name += to_string(ny);
	name += ".bmp";
	cam.SaveBitmap((char*)name.c_str());
}

void project2()
{
	// Create scene
	Scene scn;
	scn.SetSkyColor(Color(0.8f, 0.8f, 1.0f));
	
	// Create ground
	MeshObject ground;
	ground.MakeBox(5.0f, 0.1f, 5.0f);
	scn.AddObject(ground);
	
	// Create dragon
	MeshObject dragon;
	dragon.LoadPLY("dragon.ply");
	dragon.Smooth();
	
	BoxTreeObject tree;
	tree.Construct(dragon);
	scn.AddObject(tree);
	
	// Create instance
	InstanceObject inst(tree);
	glm::mat4x4 mtx = glm::eulerAngleY(PI);
	mtx[3] = glm::vec4(-0.05f, 0.0f, -0.1f, 1.0f);
	inst.SetMatrix(mtx);
	scn.AddObject(inst);
	
	// Create lights
	DirectLight sunlgt;
	sunlgt.SetBaseColor(Color(1.0f, 1.0f, 0.9f));
	sunlgt.SetIntensity(1.0f);
	sunlgt.SetDirection(glm::vec3(2.0f, -3.0f, -2.0f));
	scn.AddLight(sunlgt);

	PointLight redlgt;
	redlgt.SetBaseColor(Color(1.0f, 0.2f, 0.2f));
	redlgt.SetIntensity(0.02f);
	redlgt.SetPosition(glm::vec3(-0.2f, 0.2f, 0.2f));
	scn.AddLight(redlgt);

	PointLight bluelgt;
	bluelgt.SetBaseColor(Color(0.2f, 0.2f, 1.0f));
	bluelgt.SetIntensity(0.02f);
	bluelgt.SetPosition(glm::vec3(0.1f, 0.1f, 0.3f));//0.1f
	scn.AddLight(bluelgt);
	
	// Create camera
	Camera cam;
	//-0.1f, 0.1f, 0.2f
	cam.LookAt(glm::vec3(-0.1f, 0.1f, 0.2f), glm::vec3(-0.05f, 0.12f, 0.0f),
		glm::vec3(0, 1.0f, 0));
	cam.SetFOV(40.0f);
	cam.SetAspect(1.33f);
	cam.SetResolution(800, 600);
	
	// Render image
	cam.Render(scn);
	cam.SaveBitmap("project2.bmp");

}

void project1() {
	// Create scene
	Scene scn;
	scn.SetSkyColor(Color(0.8f, 0.9f, 1.0f));

	// Create boxes
	MeshObject box1;
	box1.MakeBox(5.0f,0.1f,5.0f);
	scn.AddObject(box1);

	MeshObject box2;
	box2.MakeBox(1.0f,1.0f,1.0f);

	// Test
	//BoxTreeObject tree;
	//tree.Construct(box1);
	//scn.AddObject(tree);

	InstanceObject inst1(box2);
	glm::mat4x4 mtx=glm::rotate(glm::mat4x4(),0.5f,glm::vec3(1,0,0));
	mtx[3][1]=1.0f;
	inst1.SetMatrix(mtx);
	scn.AddObject(inst1);

	InstanceObject inst2(box2);
	mtx=glm::rotate(glm::mat4x4(),1.0f,glm::vec3(0,1,0));
	mtx[3]=glm::vec4(-1,0,1,1);
	inst2.SetMatrix(mtx);
	scn.AddObject(inst2);

	// Create lights
	DirectLight sunlgt;
	sunlgt.SetBaseColor(Color(1.0f, 1.0f, 0.9f));
	sunlgt.SetIntensity(0.5f);
	sunlgt.SetDirection(glm::vec3(-0.5f, -1.0f, -0.5f));
	scn.AddLight(sunlgt);

	PointLight redlgt;
	redlgt.SetBaseColor(Color(1.0f, 0.2f, 0.2f));
	redlgt.SetIntensity(2.0f);
	redlgt.SetPosition(glm::vec3(2.0f, 2.0f, 0.0f));
	scn.AddLight(redlgt);

	// Create camera
	Camera cam;
	cam.LookAt(glm::vec3(2.0f, 2.0f, 5.0f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0,1,0));
	cam.SetResolution(800,600);
	cam.SetFOV(40.0f);
	cam.SetAspect(1.33f);

	// Render image
	cam.Render(scn);
	cam.SaveBitmap("project1.bmp");
	
}

////////////////////////////////////////////////////////////////////////////////
