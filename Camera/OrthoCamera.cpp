
#include "OrthoCamera.h"

	OrthoCamera::OrthoCamera(Model* Object, glm::vec3 worldUp) {
		//init camera pos
		this->cameraPos = glm::vec3(0, 10, 0.01);

		this->projectionMatrix = glm::ortho(
			-20.f, //Left
			20.f,//Right
			-20.f,//Bot
			20.f,//Top
			-20.f,//Near
			20.f //Far
		);

		this->FogMax = 500;
		this->FogMin = 0.01f;

		this->worldUp = worldUp;
		this->viewMatrix = glm::lookAt(this->cameraPos, { Object->x,  Object->y, Object->z }, this->worldUp);
		//for offseting the camera while being up to date with model pos
		this->vecOffset = { 0,0,0 };
	}

	void OrthoCamera::processEvents(glm::vec3 modelPos, glm::vec3 objDir) {
		this->updateCamera(modelPos, objDir);
	}

	void OrthoCamera::updateCamera(glm::vec3 modelPos, glm::vec3 objDir) {
		this->cameraPos[0] = modelPos.x + vecOffset.x;
		this->cameraPos[1] = modelPos.y + 15.f + vecOffset.y;
		this->cameraPos[2] = modelPos.z - 0.01f + vecOffset.z;
		this->viewMatrix = glm::lookAt(this->cameraPos, { modelPos.x + vecOffset.x, modelPos.y + vecOffset.y,  modelPos.z + vecOffset.z }, this->worldUp);
		//updates to be above model, moved around by le offsete
		
	}


