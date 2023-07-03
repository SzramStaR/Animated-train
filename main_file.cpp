/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#define GLM_FORCE_RADIANS
#define FAST_OBJ_IMPLEMENTATION
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "myTeapot.h"
#include "fast_obj.h"
float speed_x=0; //angular speed in radians
float speed_y=0; //angular speed in radians
float cameraSpeed=0.05f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float currentFrame = glfwGetTime();

float aspectRatio=1;
glm::vec3 cameraPos = glm::vec3(0.0f,0.0f,-8.0f);
glm::vec3 cameraFront = glm::vec3(0.0f,0.0f,-1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f,1.0f,0.0f);



ShaderProgram *sp; //Pointer to the shader program
ShaderProgram *sp2; //smoke
ShaderProgram *sp3;  //no lighting
ShaderProgram *sp4; //bachground

//Uncomment to draw a cube
// float* vertices=myCubeVertices;
// float* texCoords= myCubeTexCoords;
// float* colors = myCubeColors;
// float* normals = myCubeNormals;
// int vertexCount = myCubeVertexCount;

//Uncomment to draw a teapot
float* vertices = myTeapotVertices;
float* texCoords = myTeapotTexCoords;
float* colors = myTeapotColors;
float* normals = myTeapotVertexNormals;
int vertexCount = myTeapotVertexCount;

typedef struct model {
	float* vertices;
	float* tex_coordinates;
	float* normals;
	int count;
	GLuint color_tex;
	GLuint spec_tex;
}model;


model plane;
model grass_plane;
model smoke;
model train;
model wheel;
GLuint tex0;
GLuint tex1;

//Background plane arrays

float plane_vertices[] = {
	1.0f,-1.0f,0.0f,1.0f,
	-1.0f,1.0f,0.0f,1.0f,
	-1.0f,-1.0f,0.0f,1.0f,
	1.0f,-1.0f,0.0f,1.0f,
	1.0f,1.0f,0.0f,1.0f,
	-1.0f,1.0f,0.0f,1.0f
};
float plane_tex_coordinates[]{
	1.0f,1.0f,0.0f,0.0f,0.0f,1.0f,
	1.0f,1.0f,1.0f,0.0f,0.0f,0.0f
};

float plane_normals[]{
	0.0f,0.0f,-1.0f,0.0f,
	0.0f,0.0f,-1.0f,0.0f,
	0.0f,0.0f,-1.0f,0.0f,
	0.0f,0.0f,-1.0f,0.0f,
	0.0f,0.0f,-1.0f,0.0f,
	0.0f,0.0f,-1.0f,0.0f
};

int plane_vertex_count = 6;

//Grass plane arrays

float grass_vertices[] = {
	1.0f,-1.0f,0.0f,1.0f,
	-1.0f,1.0f,0.0f,1.0f,
	-1.0f,-1.0f,0.0f,1.0f,
	1.0f,-1.0f,0.0f,1.0f,
	1.0f,1.0f,0.0f,1.0f,
	-1.0f,1.0f,0.0f,1.0f
};
float grass_tex_coordinates[]{
	1.0f,1.0f,0.0f,0.0f,0.0f,1.0f,
	1.0f,1.0f,1.0f,0.0f,0.0f,0.0f
};

float grass_normals[]{
	0.0f,0.0f,-1.0f,0.0f,
	0.0f,0.0f,-1.0f,0.0f,
	0.0f,0.0f,-1.0f,0.0f,
	0.0f,0.0f,-1.0f,0.0f,
	0.0f,0.0f,-1.0f,0.0f,
	0.0f,0.0f,-1.0f,0.0f
};

int grass_vertex_count = 6;


// GLuint feedback[2];
// GLuint posBuf[2];
// GLuint velBuf[2];
// GLuint startTime[2];

// //Setup the feedback ojbects
// glGenTransfromFeedbacks(2,feedback);
// //Transfrom feedback 0
// glBindTransfromFeedback(GL_TRANSFORM_FEEDBACK, feedback[0]);
// glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER,0,posBuf[0]);
// glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER,1,velBuf[0]);
// glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER,2,startTime[0]);
// //Transform feedback 1 
// glBindTransfromFeedback(GL_TRANSFORM_FEEDBACK, feedback[1]);
// glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER,0,posBuf[1]);
// glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER,1,velBuf[1]);
// glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER,2,startTime[1]);

// const char * outputNames[] = {"Position", "Velocity", "StartTime"};
// glGenTransfromFeedbackVarying(progHandle, 3,outputNames,GL_SEPARATE_ATTRIBS);


void loadMesh(fastObjMesh* mesh,int* sz, float** vertices, float** tex_crds, float** normls, int init_alloc);

//Error processing callback procedure
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void keyCallback(GLFWwindow* window,int key,int scancode,int action,int mods) {
	//keys look around
	// float cameraSpeed = 10.0f * deltaTime;
    // if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    //     cameraPos += cameraSpeed * cameraFront;
    // if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) 
    //     cameraPos -= cameraSpeed * cameraFront;
    // if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    //     cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    // if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    //     cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	// if (action==GLFW_RELEASE) {
    //     if (key==GLFW_KEY_LEFT) cameraSpeed = 0;
    //     if (key==GLFW_KEY_RIGHT) cameraSpeed = 0;
    //     if (key==GLFW_KEY_UP) cameraSpeed = 0;
    //     if (key==GLFW_KEY_DOWN) cameraSpeed = 0;
    // }

	 if (action==GLFW_PRESS) {
        if (key==GLFW_KEY_LEFT) speed_x=-PI/2;
        if (key==GLFW_KEY_RIGHT) speed_x=PI/2;
        if (key==GLFW_KEY_UP) speed_y=PI/2;
        if (key==GLFW_KEY_DOWN) speed_y=-PI/2;
    }
    if (action==GLFW_RELEASE) {
        if (key==GLFW_KEY_LEFT) speed_x=0;
        if (key==GLFW_KEY_RIGHT) speed_x=0;
        if (key==GLFW_KEY_UP) speed_y=0;
        if (key==GLFW_KEY_DOWN) speed_y=0;
    }

}

void windowResizeCallback(GLFWwindow* window,int width,int height) {
    if (height==0) return;
    aspectRatio=(float)width/(float)height;
    glViewport(0,0,width,height);
}

GLuint readTexture(const char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);

	//Load into computer's memory
	std::vector<unsigned char> image;   //Allocate a vector for image storage
	unsigned width, height;   //Variables for image size
	//Read image
	unsigned error = lodepng::decode(image, width, height, filename);

	//Import into graphics card's memory
	glGenTextures(1, &tex); //Initialize one handle
	glBindTexture(GL_TEXTURE_2D, tex); //Activate the handle
	//Import image into graphics card's memory associated with the handle
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return tex;
}
GLuint readGrassTexture(const char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);

	//Load into computer's memory
	std::vector<unsigned char> image;   //Allocate a vector for image storage
	unsigned width, height;   //Variables for image size
	//Read image
	unsigned error = lodepng::decode(image, width, height, filename);

	//Import into graphics card's memory
	glGenTextures(1, &tex); //Initialize one handle
	glBindTexture(GL_TEXTURE_2D, tex); //Activate the handle
	//Import image into graphics card's memory associated with the handle
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	return tex;
}


//wheels
float* vertices_wheels;
float* texCoords_wheels;
float* colors_wheels;
float* normals_wheels;
int vertexCount_wheels;

float* vertices_s_wheels;
float* texCoords_s_wheels;
float* colors_s_wheels;
float* normals_s_wheels;
int vertexCount_s_wheels;

glm::vec3 wheels[16];		
float speed = PI/2;

//Initialization code procedure
void initOpenGLProgram(GLFWwindow* window) {
	//************Place any code here that needs to be executed once, at the program start************
	glClearColor(0,0,0,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glfwSetWindowSizeCallback(window,windowResizeCallback);
	glfwSetKeyCallback(window,keyCallback);
	sp=new ShaderProgram("v_simplest.glsl",NULL,"f_simplest.glsl"); //train shader
	sp2=new ShaderProgram("v_smoke2.glsl","g_smoke.glsl","f_smoke2.glsl"); //smoke shader
	sp3 = new ShaderProgram("v_grass.glsl",NULL, "f_grass.glsl"); //grass shader
	sp4 = new ShaderProgram("v_background.glsl",NULL,"f_simplest.glsl");//background shader
	train.color_tex = readTexture("steel.png");
	tex0 = readTexture("steel.png");

	fastObjMesh* mesh = fast_obj_read("wheeltest.obj");
	loadMesh(mesh,&train.count, &train.vertices, &train.tex_coordinates, &train.normals, 512);

	fastObjMesh* mesh2 = fast_obj_read("cloud.obj");
	loadMesh(mesh2,&smoke.count, &smoke.vertices, &smoke.tex_coordinates, &smoke.normals, 512);

	//Background plane
	plane.vertices = plane_vertices;
	plane.tex_coordinates = plane_tex_coordinates;
	plane.normals = plane_normals;
	plane.count = plane_vertex_count;
	plane.color_tex = readTexture("sky.png");
	plane.spec_tex = plane.color_tex;

	//Grass plane
	grass_plane.color_tex = readGrassTexture("grass_tex.png");
	grass_plane.spec_tex = readGrassTexture("fur.png");

	//Smoke
	smoke.color_tex = readTexture("Solid_white.png");
	fastObjMesh* wheel_m = fast_obj_read("train_bez.obj");
	fastObjMesh* smallwheel = fast_obj_read("wheelsmallmodel.obj");
	loadMesh(wheel_m,&wheel.count, &wheel.vertices, &wheel.tex_coordinates, &wheel.normals, 512);
	
		 wheels[4]=glm::vec3(1.226011f, -0.686409f, -0.253481f);
		 wheels[1]=glm::vec3(1.226011f, -0.686409f, 0.253481f);
		 wheels[2]=glm::vec3(0.528647f, -0.686409f, 0.253481f);
		 wheels[3]=glm::vec3(-1.092862f, -0.690224f, 0.253481f);
		 wheels[0]=glm::vec3(-1.802011f, -0.689583f, 0.253481f);
		 wheels[5]=glm::vec3(0.528647f, -0.686409f, -0.252613f);
		 wheels[6]=glm::vec3(-1.092862f, -0.690223f, -0.252613f);
		 wheels[7]=glm::vec3(-1.802011f, -0.689583f, -0.252613f);
		 wheels[8]=glm::vec3(-3.416314f, -0.818402f, -0.252353);
		 wheels[9]=glm::vec3(-5.037823f, -0.822216f, -0.252353);
		 wheels[10]=glm::vec3(-5.743930f, -0.821875f, -0.252353);
		 wheels[11]=glm::vec3(-2.717125f, -0.818061f, -0.252353);
		 wheels[12]=glm::vec3(-3.416314f, -0.818402f, 0.253220f);
		 wheels[13]=glm::vec3(-5.037823f, -0.822216f, 0.253220f);
		 wheels[14]=glm::vec3(-5.743930f, -0.821875f, 0.253221f);
		 wheels[15]=glm::vec3(-2.717125f, -0.818061f, 0.253220f);
	loadMesh(smallwheel,&vertexCount_s_wheels, &vertices_s_wheels, &texCoords_s_wheels, &normals_s_wheels, 512);
	//Background plane
	plane.vertices = plane_vertices;
	plane.tex_coordinates = plane_tex_coordinates;
	plane.normals = plane_normals;
	plane.count = plane_vertex_count;
	plane.color_tex = readTexture("sky.png");
	plane.spec_tex = plane.color_tex;




}

//Release resources allocated by the program
void freeOpenGLProgram(GLFWwindow* window) {
	//************Place any code here that needs to be executed once, after the main loop ends************
	delete sp;
	delete sp2;
	delete sp3;
	delete sp4;
}

void loadMesh(fastObjMesh* mesh, int* sz, float** vertices, float** tex_crds, float** normls, int init_alloc) {
	int index = 0;
	int len = 0;
	int coords_len = 0;
	int cap = init_alloc;
	float* verts = (float*)malloc(4 * cap * sizeof(float));
	float* nrmls = (float*)malloc(4 * cap * sizeof(float));
	float* tex_cords = (float*)malloc(4 * cap * sizeof(float));
	for (int i = 0; i < mesh->face_count; i++) {
		for (int j = 0; j < mesh->face_vertices[i] - 2; j++) {
			for (int k = 0; k < 3; k++) {
				verts[len] = mesh->positions[(int)(mesh->indices[index].p) * 3 + k];
				nrmls[len++] = mesh->normals[(int)(mesh->indices[index].n) * 3 + k];
			}
			for (int k = 0; k < 2; k++) {
				tex_cords[coords_len++] = mesh->texcoords[(int)(mesh->indices[index].t) * 2 + k];
				//printf("Adding vt%d #%d: %f\n", mesh->indices[index].t, k, mesh->texcoords[(int)(mesh->indices[index].t) * 2 + k]);
			}
			verts[len] = 1.0;
			nrmls[len++] = 0.0;
			if (len % (cap * 4) == 0) {
				verts = (float*)realloc(verts, (len + cap * 4) * sizeof(float));
				nrmls = (float*)realloc(nrmls, (len + cap * 4) * sizeof(float));
				tex_cords = (float*)realloc(tex_cords, (len + cap * 4) * sizeof(float));
			}
			for (int l = 1; l < 3; l++) {
				for (int k = 0; k < 3; k++) {
					verts[len] = mesh->positions[(int)(mesh->indices[index + j + l].p) * 3 + k];
					nrmls[len++] = mesh->normals[(int)(mesh->indices[index + j + l].n) * 3 + k];
				}
				for (int k = 0; k < 2; k++) {
					//printf("Adding vt%d #%d: %f\n", mesh->indices[index + j + l].t, k, mesh->texcoords[(int)(mesh->indices[index + j + l].t) * 2 + k]);
					tex_cords[coords_len++] = mesh->texcoords[(int)(mesh->indices[index + j + l].t) * 2 + k];
				}
				verts[len] = 1.0;
				nrmls[len++] = 0.0;
				if (len % (cap * 4) == 0) {
					verts = (float*)realloc(verts, (len + cap * 4) * sizeof(float));
					nrmls = (float*)realloc(nrmls, (len + cap * 4) * sizeof(float));
					tex_cords = (float*)realloc(tex_cords, (len + cap * 4) * sizeof(float));
				}
			}
		}
		index += (int)(mesh->face_vertices[i]);
	}
	*sz = len;
	verts = (float*)realloc(verts, 4 * len * sizeof(float));
	nrmls = (float*)realloc(nrmls, 4 * len * sizeof(float));
	tex_cords = (float*)realloc(tex_cords, 2 * len * sizeof(float));
	*vertices = verts;
	*tex_crds = tex_cords;
	*normls = nrmls;
}


//Drawing procedure
void drawScene(GLFWwindow* window,float angle_x,float angle_y,float time,float angle,float wheelAngle ) {
	//************Place any code here that draws something inside the window******************l

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 V=glm::lookAt(
        cameraPos,
        cameraPos - cameraFront,
        cameraUp); //compute view matrix
    glm::mat4 P=glm::perspective(50.0f*PI/180.0f, 1.0f, 1.0f, 50.0f); //compute projection matrix


    sp->use();//activate shading program
    //Send parameters to graphics card
    glUniformMatrix4fv(sp->u("P"),1,false,glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"),1,false,glm::value_ptr(V));
    glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
	glEnableVertexAttribArray(sp->a("texCoord0")); //Enable sending data to the attribute texCoord0
	glEnableVertexAttribArray(sp->a("normal")); //Enable sending data to the attribute color
    glm::mat4 M=glm::mat4(1.0f);

	glm::mat4 background_M = glm::translate(M,glm::vec3(0.0f,0.0f,10.0f));
	background_M = glm::scale(background_M,glm::vec3(15.0f,15.0f,1.0f));


	M=glm::rotate(M,angle_y,glm::vec3(1.0f,0.0f,0.0f)); //Compute model matrix
	M=glm::rotate(M,angle_x,glm::vec3(0.0f,1.0f,0.0f)); //Compute model matrix

	glm::mat4 grass_plane_matrix = M;
	grass_plane_matrix = glm::rotate(M, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	grass_plane_matrix = glm::scale(grass_plane_matrix,glm::vec3(15.0f,15.0f,1.0f));
	grass_plane_matrix = glm::translate(grass_plane_matrix,glm::vec3(0.0f,0.0f,1.05f));
	
	glm::mat4 Ms=glm::mat4(1.0f);
	Ms=glm::rotate(Ms,angle_y,glm::vec3(1.0f,0.0f,0.0f)); //Compute model matrix
	Ms=glm::rotate(Ms,angle_x,glm::vec3(0.0f,1.0f,0.0f)); //Compute model matrix

	glm::mat4 M_train = M; //train model
	glm::mat4 Mk1 = Ms;
    	glVertexAttribPointer(sp->a("vertex"),4,GL_FLOAT,false,0,wheel.vertices); //Specify source of the data for the attribute vertex
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, wheel.tex_coordinates); //Specify source of the data for the attribute texCoord0
		glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, wheel.normals); //Specify source of the data for the attribute normal
		glUniformMatrix4fv(sp->u("M"),1,false,glm::value_ptr(M));
		glDrawArrays(GL_TRIANGLES,0,wheel.count); //Draw the object

	for (int j=0;j<=7;j++){
		Mk1 = Ms;
		Mk1 = glm::translate(Mk1,wheels[j]);
		if (j>=4){
			Mk1 = glm::rotate(Mk1,PI,glm::vec3(0.0f,1.0f,0.0f));
			Mk1 = glm::rotate(Mk1, wheelAngle, glm::vec3(0.0f, 0.0f, 1.0f));
		}
		else {
			Mk1 = glm::rotate(Mk1, -wheelAngle, glm::vec3(0.0f, 0.0f, 1.0f));
		}
    	glVertexAttribPointer(sp->a("vertex"),4,GL_FLOAT,false,0,train.vertices); //Specify source of the data for the attribute vertex
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, train.tex_coordinates); //Specify source of the data for the attribute texCoord0
		glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, train.normals); //Specify source of the data for the attribute normal
		
		glUniformMatrix4fv(sp->u("M"),1,false,glm::value_ptr(Mk1));
		
		glDrawArrays(GL_TRIANGLES,0,train.count); //Draw the object
	}
	glm::mat4 Mk2;
		for (int j=8;j<=15;j++){
		Mk2 = Ms;
		Mk2 = glm::translate(Mk2,wheels[j]);
		if (j>=12){
			Mk2 = glm::rotate(Mk2,PI,glm::vec3(0.0f,1.0f,0.0f));
			Mk2 = glm::rotate(Mk2, wheelAngle, glm::vec3(0.0f, 0.0f, 1.0f));
		}
		else {
			Mk2 = glm::rotate(Mk2, -wheelAngle, glm::vec3(0.0f, 0.0f, 1.0f));
		}
		
		
		glUniformMatrix4fv(sp->u("M"),1,false,glm::value_ptr(Mk2));
		glVertexAttribPointer(sp->a("vertex"),4,GL_FLOAT,false,0,vertices_s_wheels); //Specify source of the data for the attribute vertex
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoords_s_wheels); //Specify source of the data for the attribute texCoord0
		glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals_s_wheels); //Specify source of the data for the attribute normal
		glDrawArrays(GL_TRIANGLES,0,vertexCount_s_wheels); //Draw the object
	}
    
	glUniform1i(sp->u("textureMap0"), 0); // Associate sampler textureMap0 with the 0-th texturing unit
	glActiveTexture(GL_TEXTURE0); //Assign texture tex0 to the 0-th texturing unit
	glBindTexture(GL_TEXTURE_2D, train.color_tex);

	// glUniform1i(sp->u("textureMap1"), 1); // Associate sampler textureMap1 with the 1-st texturing unit
	// glActiveTexture(GL_TEXTURE1); //Assign texture tex1 to the 1-st texturing unit
	// glBindTexture(GL_TEXTURE_2D, tex1);

   

	glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
	glDisableVertexAttribArray(sp->a("texCoord0")); //Disable sending data to the attribute texCoord0
	glDisableVertexAttribArray(sp->a("normal")); //Disable sending data to the attribute normal



	//Background drawing

	sp4->use();	

	glUniformMatrix4fv(sp4->u("P"),1,false,glm::value_ptr(P));
    glUniformMatrix4fv(sp4->u("V"),1,false,glm::value_ptr(V));

	glUniformMatrix4fv(sp4->u("M"),1,false,glm::value_ptr(background_M));


    glEnableVertexAttribArray(sp4->a("vertex")); //Enable sending data to the attribute vertex
    glVertexAttribPointer(sp4->a("vertex"),4,GL_FLOAT,false,0,plane_vertices); //Specify source of the data for the attribute vertex

	glEnableVertexAttribArray(sp4->a("texCoord0")); //Enable sending data to the attribute texCoord0
	glVertexAttribPointer(sp4->a("texCoord0"), 2, GL_FLOAT, false, 0, plane_tex_coordinates); //Specify source of the data for the attribute texCoord0

	glEnableVertexAttribArray(sp4->a("normal")); //Enable sending data to the attribute color
	glVertexAttribPointer(sp4->a("normal"), 4, GL_FLOAT, false, 0, plane_normals); //Specify source of the data for the attribute normal

	glUniform1i(sp4->u("textureMap0"), 0); // Associate sampler textureMap0 with the 0-th texturing unit
	glActiveTexture(GL_TEXTURE0); //Assign texture tex0 to the 0-th texturing unit
	glBindTexture(GL_TEXTURE_2D, plane.color_tex);

	// glUniform1i(sp->u("textureMap1"), 1); // Associate sampler textureMap1 with the 1-st texturing unit
	// glActiveTexture(GL_TEXTURE1); //Assign texture tex1 to the 1-st texturing unit
	// glBindTexture(GL_TEXTURE_2D, tex1);

    glDrawArrays(GL_TRIANGLES,0,plane_vertex_count); //Draw the object


	glDisableVertexAttribArray(sp4->a("vertex")); //Disable sending data to the attribute vertex
	glDisableVertexAttribArray(sp4->a("texCoord0")); //Disable sending data to the attribute texCoord0
	glDisableVertexAttribArray(sp4->a("normal")); //Disable sending data to the attribute normal

	sp3 -> use();
	glUniform1f(sp3->u("maxGrassLength"),0.1);
	glUniform1f(sp3->u("maxLayer"),100);

	//Drawing grass plane
	glUniformMatrix4fv(sp3->u("P"),1,false,glm::value_ptr(P));
    glUniformMatrix4fv(sp3->u("V"),1,false,glm::value_ptr(V));
	glUniformMatrix4fv(sp3->u("M"),1,false,glm::value_ptr(grass_plane_matrix));


    glEnableVertexAttribArray(sp3->a("vertex")); //Enable sending data to the attribute vertex
    glVertexAttribPointer(sp3->a("vertex"),4,GL_FLOAT,false,0,grass_vertices); //Specify source of the data for the attribute vertex

	glEnableVertexAttribArray(sp3->a("texCoord0")); //Enable sending data to the attribute texCoord0
	glVertexAttribPointer(sp3->a("texCoord0"), 2, GL_FLOAT, false, 0, grass_tex_coordinates); //Specify source of the data for the attribute texCoord0

	glEnableVertexAttribArray(sp3->a("normal")); //Enable sending data to the attribute color
	glVertexAttribPointer(sp3->a("normal"), 4, GL_FLOAT, false, 0, grass_normals); //Specify source of the data for the attribute normal

	glUniform1i(sp3->u("textureMap0"), 0); // Associate sampler textureMap0 with the 0-th texturing unit
	glActiveTexture(GL_TEXTURE0); //Assign texture tex0 to the 0-th texturing unit
	glBindTexture(GL_TEXTURE_2D, grass_plane.color_tex);
	glUniform1i(sp3->u("textureMap1"), 1); // Associate sampler textureMap1 with the 1-st texturing unit
	glActiveTexture(GL_TEXTURE1); //Assign texture tex1 to the 1-st texturing unit
	glBindTexture(GL_TEXTURE_2D, grass_plane.spec_tex);

    glDrawArraysInstanced(GL_TRIANGLES,0,grass_vertex_count,100); //Draw the object



    // glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
	// glDisableVertexAttribArray(sp->a("texCoord0")); //Disable sending data to the attribute texCoord0
	// glDisableVertexAttribArray(sp->a("normal")); //Disable sending data to the attribute normal

	glDisableVertexAttribArray(sp3->a("vertex")); //Disable sending data to the attribute vertex
	glDisableVertexAttribArray(sp3->a("texCoord0")); //Disable sending data to the attribute texCoord0
	glDisableVertexAttribArray(sp3->a("normal")); //Disable sending data to the attribute normal

	//Smoke drawing
	sp2->use();
	float smokeTime = glm::clamp(time,0.0f,0.3f);
	float scale_factor = 0.2f;  // Scale the cube to half its original size
	
	M_train = glm::scale(M, glm::vec3(scale_factor));
	M_train = glm::translate(M_train, glm::vec3(6.5f,6.0f,0.0f));

	glUniform1f(sp2->u("amount"),smokeTime);

	glUniformMatrix4fv(sp2->u("P"),1,false,glm::value_ptr(P));
    glUniformMatrix4fv(sp2->u("V"),1,false,glm::value_ptr(V));
	glUniformMatrix4fv(sp2->u("M"),1,false,glm::value_ptr(M_train));

	glEnableVertexAttribArray(sp2->a("vertex")); //Enable sending data to the attribute vertex
    glVertexAttribPointer(sp2->a("vertex"),4,GL_FLOAT,false,0,vertices); //Specify source of the data for the attribute vertex

	glEnableVertexAttribArray(sp2->a("texCoord0")); //Enable sending data to the attribute texCoord0
	glVertexAttribPointer(sp2->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoords); //Specify source of the data for the attribute texCoord0

	glEnableVertexAttribArray(sp2->a("normal")); //Enable sending data to the attribute color
	glVertexAttribPointer(sp2->a("normal"), 4, GL_FLOAT, false, 0, normals); //Specify source of the data for the attribute normal

	glUniform1i(sp2->u("textureMap0"), 0); // Associate sampler textureMap0 with the 0-th texturing unit
	glActiveTexture(GL_TEXTURE0); //Assign texture tex0 to the 0-th texturing unit
	glBindTexture(GL_TEXTURE_2D, tex0);


	// glUniform1i(sp2->u("textureMap1"), 1); // Associate sampler textureMap1 with the 1-st texturing unit
	// glActiveTexture(GL_TEXTURE1); //Assign texture tex1 to the 1-st texturing unit
	// glBindTexture(GL_TEXTURE_2D, tex1);

    glDrawArrays(GL_TRIANGLES,0,vertexCount); //Draw the object

	scale_factor = 0.8f;

	M_train = glm::scale(M_train, glm::vec3(scale_factor));
	M_train = glm::translate(M_train, glm::vec3(-1.0f,2.0f,0.0f));

	glUniform1f(sp2->u("amount"),smokeTime);

	glUniformMatrix4fv(sp2->u("P"),1,false,glm::value_ptr(P));
    glUniformMatrix4fv(sp2->u("V"),1,false,glm::value_ptr(V));
	glUniformMatrix4fv(sp2->u("M"),1,false,glm::value_ptr(M_train));

	glEnableVertexAttribArray(sp2->a("vertex")); //Enable sending data to the attribute vertex
    glVertexAttribPointer(sp2->a("vertex"),4,GL_FLOAT,false,0,vertices); //Specify source of the data for the attribute vertex

	glEnableVertexAttribArray(sp2->a("texCoord0")); //Enable sending data to the attribute texCoord0
	glVertexAttribPointer(sp2->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoords); //Specify source of the data for the attribute texCoord0

	glEnableVertexAttribArray(sp2->a("normal")); //Enable sending data to the attribute color
	glVertexAttribPointer(sp2->a("normal"), 4, GL_FLOAT, false, 0, normals); //Specify source of the data for the attribute normal

	glUniform1i(sp2->u("textureMap0"), 0); // Associate sampler textureMap0 with the 0-th texturing unit
	glActiveTexture(GL_TEXTURE0); //Assign texture tex0 to the 0-th texturing unit
	glBindTexture(GL_TEXTURE_2D, tex0);


	// glUniform1i(sp2->u("textureMap1"), 1); // Associate sampler textureMap1 with the 1-st texturing unit
	// glActiveTexture(GL_TEXTURE1); //Assign texture tex1 to the 1-st texturing unit
	// glBindTexture(GL_TEXTURE_2D, tex1);

    glDrawArrays(GL_TRIANGLES,0,vertexCount); //Draw the object

	scale_factor = 0.8f;

	M_train = glm::scale(M_train, glm::vec3(scale_factor));
	M_train = glm::translate(M_train, glm::vec3(-1.0f,2.0f,0.0f));

	glUniform1f(sp2->u("amount"),smokeTime);

	glUniformMatrix4fv(sp2->u("P"),1,false,glm::value_ptr(P));
    glUniformMatrix4fv(sp2->u("V"),1,false,glm::value_ptr(V));
	glUniformMatrix4fv(sp2->u("M"),1,false,glm::value_ptr(M_train));

	glEnableVertexAttribArray(sp2->a("vertex")); //Enable sending data to the attribute vertex
    glVertexAttribPointer(sp2->a("vertex"),4,GL_FLOAT,false,0,vertices); //Specify source of the data for the attribute vertex

	glEnableVertexAttribArray(sp2->a("texCoord0")); //Enable sending data to the attribute texCoord0
	glVertexAttribPointer(sp2->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoords); //Specify source of the data for the attribute texCoord0

	glEnableVertexAttribArray(sp2->a("normal")); //Enable sending data to the attribute color
	glVertexAttribPointer(sp2->a("normal"), 4, GL_FLOAT, false, 0, normals); //Specify source of the data for the attribute normal

	glUniform1i(sp2->u("textureMap0"), 0); // Associate sampler textureMap0 with the 0-th texturing unit
	glActiveTexture(GL_TEXTURE0); //Assign texture tex0 to the 0-th texturing unit
	glBindTexture(GL_TEXTURE_2D, tex0);


	// glUniform1i(sp2->u("textureMap1"), 1); // Associate sampler textureMap1 with the 1-st texturing unit
	// glActiveTexture(GL_TEXTURE1); //Assign texture tex1 to the 1-st texturing unit
	// glBindTexture(GL_TEXTURE_2D, tex1);

    glDrawArrays(GL_TRIANGLES,0,vertexCount); //Draw the object
	

	glDisableVertexAttribArray(sp2->a("vertex")); //Disable sending data to the attribute vertex
	glDisableVertexAttribArray(sp2->a("texCoord0")); //Disable sending data to the attribute texCoord0
	glDisableVertexAttribArray(sp2->a("normal")); //Disable sending data to the attribute normal


    glfwSwapBuffers(window); //Copy back buffer to front buffer
}

int main(void)
{
	GLFWwindow* window; //Pointer to object that represents the application window

	glfwSetErrorCallback(error_callback);//Register error processing callback procedure

	if (!glfwInit()) { //Initialize GLFW library
		fprintf(stderr, "Can't initialize GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(800, 600, "OpenGL", NULL, NULL);  //Create a window 500pxx500px titled "OpenGL" and an OpenGL context associated with it.

	if (!window) //If no window is opened then close the program
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Since this moment OpenGL context corresponding to the window is active and all OpenGL calls will refer to this context.
	glfwSwapInterval(1); //During vsync wait for the first refresh

	GLenum err;
	if ((err=glewInit()) != GLEW_OK) { //Initialize GLEW library
		fprintf(stderr, "Can't initialize GLEW: %s\n", glewGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Call initialization procedure


	float angle_x=0; //current rotation angle of the object, x axis
	float angle_y=0; //current rotation angle of the object, y axis
	float time = 0.0f;
	glfwSetTime(0); //Zero the timer
	//Main application loop
	float angle = 0; //declare variable for storing current rotation angle
	float wheelAngle = 0;
	while (!glfwWindowShouldClose(window)) //As long as the window shouldnt be closed yet...
	{	
		
		//Time calculations for the smoke effect
		if(time>=0.3f){
			time = 0.0f;
		}else
		time += 0.1f;
	angle += speed * glfwGetTime(); //Compute an angle by which the object was rotated during the previous frame
	wheelAngle += PI / 6 * glfwGetTime(); //Compute an angle by which the object was rotated during the previous frame
        angle_x+=speed_x*glfwGetTime(); //Add angle by which the object was rotated in the previous iteration
		angle_y+=speed_y*glfwGetTime(); //Add angle by which the object was rotated in the previous iteration
        glfwSetTime(0); //Zero the timer
		//key look around		
		// float currentFrame = static_cast<float>(glfwGetTime());
		// deltaTime = currentFrame - lastFrame;
		// lastFrame = currentFrame;
		drawScene(window,angle_x,angle_y,time,angle,wheelAngle); //Execute drawing procedure
		glfwPollEvents(); //Process callback procedures corresponding to the events that took place up to now
	}
	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Delete OpenGL context and the window.
	glfwTerminate(); //Free GLFW resources
	exit(EXIT_SUCCESS);
}
