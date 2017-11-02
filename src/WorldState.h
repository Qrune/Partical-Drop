#ifndef __WORLDSTATE_H
#define __WORLDSTATE_H
#include "Model.h"

#define NUM_TRACKED_FRAMES 10

class WorldState
{
private:
	float frameTimes[NUM_TRACKED_FRAMES];
	bool running;
    int currentMode;
	//TODO1: Change this to a vector of models
	// Fix the fallout
	vector<Model> models;

public:
	
	float currentTime;
	
	glm::vec3 cameraPos;
	glm::vec3 cameraLook;
	glm::vec3 cameraUp;
	
	glm::vec4 lightPos;
	glm::vec3 lightIntensity;
	
	glm::mat4 lightRotate;
	glm::mat4 lightIncrement;
	glm::mat4 modelIncrementRot;
	glm::mat4 modelTransform;
	glm::mat4 cameraMatrix;
	glm::mat4 lightView;
	
	bool lightRotating;
	bool modelRotating;
    bool moveEnable;

	float cursorScrollAmount;
	float center[3];
	float lastClickPos[2];
	float lastFrameDragPos[2];
	float cursorDragAmount[2];
	float cursorAbsolutePos[2];
	int currentRes[2];
	bool mouseButtonDown;

public:
	WorldState()
	{ }

	void init()
	{
		for(size_t i=0; i<NUM_TRACKED_FRAMES; i++)
			frameTimes[i] = 0.0f;
		
		running = true;
		Model model = Model();
		model.init("resources/material_test.obj");
		model.setupAttributeBuffers();
		
		glm::vec3 center = model.getCentroid();
		glm::vec3 max = model.getMaxBound();
		glm::vec3 min = model.getMinBound();
		glm::vec3 dim = model.getDimension();
		glm::vec3 toMax = max-center;
		printf("model loaded\n");
		printf("min [%.2f %.2f %.2f]\n", min[0], min[1], min[2]);
		printf("max [%.2f %.2f %.2f]\n", max[0], max[1], max[2]);
		printf("cen [%.2f %.2f %.2f]\n", center[0], center[1], center[2]);
		printf("dim [%.2f %.2f %.2f]\n", dim[0], dim[1], dim[2]);
		float camDistance = std::max(dim[0], dim[1]);
		cameraPos = glm::vec3(0,(center+(toMax*5.0f))[1],camDistance);
		cameraLook = glm::vec3(0,0,0);
		cameraUp = glm::vec3(0,1,0);
		
		lightPos = glm::vec4((center+(toMax*2.0f)), 1);
		lightPos[1] = (center+(toMax*6.0f))[1];
		lightIntensity = glm::vec3(1,1,1);
		
		lightRotate = glm::mat4(1);
		lightIncrement = glm::rotate(glm::mat4(1), -0.05f, glm::vec3(0,1,0));
		
		modelIncrementRot = glm::rotate(glm::mat4(1), 0.02f, glm::vec3(0,1,0));
		modelTransform = glm::translate(glm::mat4(1), -model.getCentroid());
        models.push_back(model);
		
		//TODO2: load teapot.obj
		// Translate its centroid to the origin, then to 2.22,0,2.22
        Model model2 = Model();
        model2.init("resources/teapot.obj");
        model2.setupAttributeBuffers();
        models.push_back(model2);
        glm::vec3 center2 = model2.getCentroid();
        glm::mat4 trans2 = glm::translate(glm::mat4(1), -glm::vec3(2.22, 2, 2.22));
        trans2 = glm::scale(trans2, glm::vec3(0.5,0.5,0.5));
        glm::mat4 modelTransform2 = glm::translate(glm::mat4(1), -model2.getCentroid());
        modelTransform2 = trans2*modelTransform2;
        model2.setTransform(trans2);
        models.push_back(model2);
        model2.setMoveIndex(1);
        
        
		
		lightRotating = false;
		modelRotating = false;
	}
	
	void updateFrameTime(float timeAsSeconds)
	{
		for(size_t i=1; i<NUM_TRACKED_FRAMES; i++)
			frameTimes[i] = frameTimes[i-1];
		frameTimes[0] = timeAsSeconds;
	}
	
	void printFPS() const
	{
		float sum = 0.0f;
		float avg = 0.0f;
		float fps = 0.0f;
		
		for(size_t i=0; i<NUM_TRACKED_FRAMES; i++)
			sum += frameTimes[i];
		
		avg = sum / NUM_TRACKED_FRAMES;
		fps = 1.0f / avg;
		printf("fps %f\n", fps);
	}
	
	Model const & getModel(int index) const
	{ return models.at(index); }
	
	void setRunning(bool r)
	{ running = r; }

	bool isRunning() const
	{ return running; }

	float getCurrentTime() const
	{ return this->currentTime; }

	void timeStep(float t)
	{
		float elapsed = t - this->currentTime;
		this->updateFrameTime(elapsed);
		
		//spin light
		if(lightRotating)
			lightRotate = lightIncrement * lightRotate;
		
		//spin model
		if(modelRotating)
			modelTransform = modelIncrementRot * modelTransform;
		
		glm::vec3 currentLightPos = glm::vec3(lightRotate*lightPos);
		lightView = glm::lookAt(currentLightPos, cameraLook, cameraUp);
		
		this->currentTime = t;
        //call move function for each model
        for (int i=0; i<models.size(); i++){
            models.at(i).Move();
        }
        
	}
	
	Model & getModel(int index)
	{ return models.at(index); }
	
	glm::mat4 getModelTransform(int index) const
	{ return models.at(index).getTransform(); }
	
	glm::mat4 getLightRotate() const
	{ return lightRotate; }
    
    void switchShadingMode(){
        this->currentMode ++;
        this->currentMode = this->currentMode % 3;
        printf("current Mode is %d\n",this->currentMode);
    }
    int getShadingMode() const{
        return this->currentMode;
    }
	glm::vec4 getLightPos() const
	{ return this->lightPos; }
	
	glm::vec3 getLightIntensity() const
	{ return this->lightIntensity; }
	
	glm::mat4 getCameraMatrix() const
	{ return glm::lookAt(cameraPos, cameraLook, cameraUp); }
	
	glm::mat4 getLightViewMatrix() const
	{ return lightView; }
	
	glm::vec4 getCameraPos() const
	{ return glm::vec4(this->cameraPos, 1); }
	
	void toggleModelRotate()
	{ modelRotating = !modelRotating; }
	
	void toggleLightRotate()
	{ lightRotating = !lightRotating; }
	
	void zoomCamera(int delta)
	{
		float d = pow(0.95, delta);
		printf("%f\n", d);
		cameraPos = cameraPos * d;
	}
    //start move?
    void startMove(){
        this->moveEnable = !this->moveEnable;
    }
    bool moveEnabled() const{
        return this->moveEnable;
    }
};

#endif
