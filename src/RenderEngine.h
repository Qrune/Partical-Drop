#ifndef __RENDERENGINE
#define __RENDERENGINE

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "GLHelper.h"
#include "WorldState.h"

#define TEX_TYPE GL_TEXTURE_2D
//#define TEX_TYPE GL_TEXTURE_RECTANGLE

class RenderEngine
{
public:
	RenderEngine()
	{
		initialized = false;
	}

	~RenderEngine()
	{
		if(initialized)
		{
			// Clean up the buffers
			//glDeleteBuffers(1, &positionBuffer);
			//glDeleteBuffers(1, &colorBuffer);
		}
	}

	void init(WorldState & state)
	{
		initialized = true;
		
		glEnable(GL_DEPTH_TEST);
		
		//glCullFace(GL_BACK);
		//glEnable(GL_CULL_FACE);
		
		setupShader();
		//setupBuffers(state.getModel());
	}

	void display(WorldState & state)
	{
		size_t shaderId;
        shaderId = 0;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(shaderProg[shaderId]);
        uploadUniforms(shaderProg[shaderId], state,0);
        state.getModel(0).draw(shaderProg[shaderId]);
        
//        glEnable(GL_DEPTH_TEST);
//        //how to cull front face?
//        glEnable(GL_CULL_FACE);
//        glCullFace(GL_BACK);
//
//        glEnable(GL_BLEND);
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
		
        uploadUniforms(shaderProg[shaderId], state, 1);
		
		//draw
		
		state.getModel(1).draw(shaderProg[shaderId]);
        uploadUniforms(shaderProg[shaderId], state, 2);
        state.getModel(2).draw(shaderProg[shaderId]);
        
        for (int i=3; i<42; i++){
            uploadUniforms(shaderProg[shaderId], state,i);
            state.getModel(i).draw(shaderProg[shaderId]);
        }
		glUseProgram(0);
		checkGLError("model");
        if (state.moveEnabled()){
            printf("enable");
        }
	}

private:
	bool initialized;
	GLuint shaderProg[3];


	glm::vec2 size;
	
	float _near;
	float _far;
	float fov;

	void setupShader()
	{
		char const * vertPath = "resources/reflectance.vert";
		char const * fragPath = "resources/reflectance.frag";
		shaderProg[0] = ShaderManager::shaderFromFile(&vertPath, &fragPath, 1, 1);

		checkGLError("shader");
	}

	void setupBuffers(Model & model)
	{



		checkGLError("setup");
	}

	void uploadUniforms(GLuint shaderId, WorldState const & state, int index)
	{
		glm::vec3 dim = state.getModel(index).getDimension();
		float maxDim = std::max(dim[0], std::max(dim[1], dim[2]));
		_near = maxDim*0.1f;
		_far  = maxDim*3.0f;
		fov = 0.9f;
		
		glm::mat4 P = glm::perspective(1.0f, fov, _near, _far);
		glm::mat4 mT = state.getModel(index).getTransform();
		glm::mat4 C = state.getCameraMatrix();
		glm::mat4 M = C*mT;
		glm::mat3 N = glm::inverseTranspose(glm::mat3(M));
		glm::vec4 lightPos = state.getLightPos();
		glm::vec4 camPos = state.getCameraPos();
		glm::mat4 Lr = state.getLightRotate();
		glm::mat4 Lv = state.getLightViewMatrix();
		glm::mat4 Lp;
		Lp = glm::perspective(1.0f, fov, _near, _far);
		Lp = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, 0.0f, 50.0f);

		//hacky light source size change
		GLfloat maxDis = state.getModel(index).getDimension()[2] * 3;
		GLfloat distScale = 1.0f / (glm::length(Lr*lightPos - camPos) / maxDis);
		glPointSize(glm::mix(1.0f, 10.0f, distScale));

		//printf("cam %f %f %f\n", camPos[0], camPos[1], camPos[2]);
		//printf("light %f %f %f\n", lightPos[0], lightPos[1], lightPos[2]);
		
		glUniformMatrix4fv(glGetUniformLocation(shaderId, "P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shaderId, "Lp"), 1, GL_FALSE, &Lp[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shaderId, "C"), 1, GL_FALSE, &C[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shaderId, "mT"), 1, GL_FALSE, &mT[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shaderId, "M"), 1, GL_FALSE, &M[0][0]);
		glUniformMatrix3fv(glGetUniformLocation(shaderId, "N"), 1, GL_FALSE, &N[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shaderId, "Lr"), 1, GL_FALSE, &Lr[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shaderId, "Lv"), 1, GL_FALSE, &Lv[0][0]);
		glUniform4fv(glGetUniformLocation(shaderId, "lightPos"), 1, &lightPos[0]);
		glUniform4fv(glGetUniformLocation(shaderId, "camPos"), 1, &camPos[0]);
        glUniform1i(glGetUniformLocation(shaderId,"curMode"),state.getShadingMode());

//		glUniform1f(glGetUniformLocation(shaderId, "elapsedTime"), state.currentTime);
//		glUniform1f(glGetUniformLocation(shaderId, "near"), _near);
//		glUniform1f(glGetUniformLocation(shaderId, "far"), _far);
//		glUniform1f(glGetUniformLocation(shaderId, "fov"), fov);
//		glUniform1f(glGetUniformLocation(shaderId, "cursorScrollAmount"), state.cursorScrollAmount);
		glUniform2f(glGetUniformLocation(shaderId, "resolution"), state.currentRes[0], state.currentRes[1]);
//		glUniform3f(glGetUniformLocation(shaderId, "modelCenter"),  state.center[0], state.center[1], state.center[2]);
//		glUniform3f(glGetUniformLocation(shaderId, "lookAtPos"),  state.cameraLook[0], state.cameraLook[1], state.cameraLook[2]);
//		glUniform3f(glGetUniformLocation(shaderId, "cameraUp"),  state.cameraUp[0], state.cameraUp[1], state.cameraUp[2]);
//		glUniform2f(glGetUniformLocation(shaderId, "cursorAbsolutePos"), state.cursorAbsolutePos[0], state.cursorAbsolutePos[1]);
//		glUniform2f(glGetUniformLocation(shaderId, "cursorDragAmount"), state.cursorDragAmount[0], state.cursorDragAmount[1]);
//		glUniform2f(glGetUniformLocation(shaderId, "lastClickPos"), state.lastClickPos[0], state.lastClickPos[1]);
//		glUniform2f(glGetUniformLocation(shaderId, "lastFrameDragPos"), state.lastFrameDragPos[0], state.lastFrameDragPos[1]);
//		glUniform1i(glGetUniformLocation(shaderId, "mouseButtonDown"), state.mouseButtonDown);
	}

};

#endif
