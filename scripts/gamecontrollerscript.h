#pragma once
#include "../Core/scripting/ntshengn_script.h"
#include "clownscript.h"
#include "signscript.h"
#include <array>
#include <random>

using namespace NtshEngn;
struct GameControllerScript : public Script {
	NTSHENGN_SCRIPT(GameControllerScript);

	Entity createClown(int type, int color, Math::vec3 position, float scale) {
		Entity newClown = createEntity();

		Transform& clownTransform = getEntityComponent<Transform>(newClown);
		clownTransform.position = position;
		clownTransform.scale = Math::vec3(scale);

		Renderable clownRenderable;
		clownRenderable.model = clownModels[type];
		clownRenderable.modelPrimitiveIndex = 0;
		addEntityComponent(newClown, clownRenderable);

		Collidable clownCollidable;
		clownCollidable.collider = std::make_unique<ColliderSphere>();
		ColliderSphere* collidableSphere = static_cast<ColliderSphere*>(clownCollidable.collider.get());
		collidableSphere->radius = 1.0f;
		addEntityComponent(newClown, clownCollidable);

		Rigidbody clownRigidbody;
		clownRigidbody.restitution = 1.0f;
		addEntityComponent(newClown, clownRigidbody);

		Scriptable clownScriptable;
		ClownScript* clownScript = createScript<ClownScript>();
		clownScriptable.script = clownScript;
		clownScript->boingSound = clownBoingSound;
		clownScript->type = type;
		clownScript->color = color;
		addEntityComponent(newClown, clownScriptable);

		return newClown;
	}

	Entity createSign(int location, int color) {
		Entity newSign = createEntity();

		Transform& signTransform = getEntityComponent<Transform>(newSign);
		if (location == 0) {
			signTransform.position = signLeftPosition;
		}
		else if (location == 1) {
			signTransform.position = signRightPosition;
		}

		Renderable signRenderable;
		if (location == 0) {
			if (color == -1) {
				signRenderable.model = signRobeNormal;
			}
			else if (color == 0) {
				signRenderable.model = signRobeRed;
			}
			else if (color == 1) {
				signRenderable.model = signRobeBlue;
			}
			else if (color == 2) {
				signRenderable.model = signRobeGreen;
			}
			else if (color == 3) {
				signRenderable.model = signRobeYellow;
			}
			else if (color == 4) {
				signRenderable.model = signRobePink;
			}
		}
		else if (location == 1) {
			if (color == -1) {
				signRenderable.model = signDotNormal;
			}
			else if (color == 0) {
				signRenderable.model = signDotRed;
			}
			else if (color == 1) {
				signRenderable.model = signDotBlue;
			}
			else if (color == 2) {
				signRenderable.model = signDotGreen;
			}
			else if (color == 3) {
				signRenderable.model = signDotYellow;
			}
			else if (color == 4) {
				signRenderable.model = signDotPink;
			}
		}
		signRenderable.modelPrimitiveIndex = 0;
		addEntityComponent(newSign, signRenderable);

		Scriptable signScriptable;
		signScriptable.script = createScript<SignScript>();
		addEntityComponent(newSign, signScriptable);

		return newSign;
	}

	void init() {
		font = getFontID(*loadFont("assets/fonts/JetBrainsMono-Regular.ttf", 32));
		fontBig = getFontID(*loadFont("assets/fonts/JetBrainsMono-Regular.ttf", 128));

		camera = findEntityByName("Camera");

		buttonRed = findEntityByName("ButtonRed");
		buttonBlue = findEntityByName("ButtonBlue");
		buttonGreen = findEntityByName("ButtonGreen");
		buttonYellow = findEntityByName("ButtonYellow");
		buttonPink = findEntityByName("ButtonPink");

		signLeft = findEntityByName("SignLeft");
		signRight = findEntityByName("SignRight");

		signRobeNormal = loadModel("assets/models/sign_robe.gltf");
		signRobeRed = loadModel("assets/models/sign_robe_red.gltf");
		signRobeBlue = loadModel("assets/models/sign_robe_blue.gltf");
		signRobeGreen = loadModel("assets/models/sign_robe_green.gltf");
		signRobeYellow = loadModel("assets/models/sign_robe_yellow.gltf");
		signRobePink = loadModel("assets/models/sign_robe_pink.gltf");

		signDotNormal = loadModel("assets/models/sign_dot.gltf");
		signDotRed = loadModel("assets/models/sign_dot_red.gltf");
		signDotBlue = loadModel("assets/models/sign_dot_blue.gltf");
		signDotGreen = loadModel("assets/models/sign_dot_green.gltf");
		signDotYellow = loadModel("assets/models/sign_dot_yellow.gltf");
		signDotPink = loadModel("assets/models/sign_dot_pink.gltf");

		buttonRedPosition = getEntityComponent<Transform>(buttonRed).position;
		buttonBluePosition = getEntityComponent<Transform>(buttonBlue).position;
		buttonGreenPosition = getEntityComponent<Transform>(buttonGreen).position;
		buttonYellowPosition = getEntityComponent<Transform>(buttonYellow).position;
		buttonPinkPosition = getEntityComponent<Transform>(buttonPink).position;
		signLeftPosition = getEntityComponent<Transform>(signLeft).position;
		signRightPosition = getEntityComponent<Transform>(signRight).position;
		bolLeftPos = getEntityComponent<Transform>(findEntityByName("BolLeft")).position;
		bolRightPos = getEntityComponent<Transform>(findEntityByName("BolRight")).position;
		clownMixSpawn = getEntityComponent<Transform>(findEntityByName("Tube")).position + Math::vec3(0.0f, 0.0f, 5.0f);

		clownModels[0] = loadModel("assets/models/clown_red.gltf");
		clownModels[1] = loadModel("assets/models/clown_blue.gltf");
		clownModels[2] = loadModel("assets/models/clown_green.gltf");
		clownModels[3] = loadModel("assets/models/clown_yellow.gltf");
		clownModels[4] = loadModel("assets/models/clown_pink.gltf");

		clownModels[5] = loadModel("assets/models/clown_redbluedot.gltf");
		clownModels[6] = loadModel("assets/models/clown_redgreendot.gltf");
		clownModels[7] = loadModel("assets/models/clown_redyellowdot.gltf");
		clownModels[8] = loadModel("assets/models/clown_redpinkdot.gltf");

		clownModels[9] = loadModel("assets/models/clown_bluereddot.gltf");
		clownModels[10] = loadModel("assets/models/clown_bluegreendot.gltf");
		clownModels[11] = loadModel("assets/models/clown_blueyellowdot.gltf");
		clownModels[12] = loadModel("assets/models/clown_bluepinkdot.gltf");

		clownModels[13] = loadModel("assets/models/clown_greenreddot.gltf");
		clownModels[14] = loadModel("assets/models/clown_greenbluedot.gltf");
		clownModels[15] = loadModel("assets/models/clown_greenyellowdot.gltf");
		clownModels[16] = loadModel("assets/models/clown_greenpinkdot.gltf");

		clownModels[17] = loadModel("assets/models/clown_yellowreddot.gltf");
		clownModels[18] = loadModel("assets/models/clown_yellowbluedot.gltf");
		clownModels[19] = loadModel("assets/models/clown_yellowgreendot.gltf");
		clownModels[20] = loadModel("assets/models/clown_yellowpinkdot.gltf");

		clownModels[21] = loadModel("assets/models/clown_pinkreddot.gltf");
		clownModels[22] = loadModel("assets/models/clown_pinkbluedot.gltf");
		clownModels[23] = loadModel("assets/models/clown_pinkgreendot.gltf");
		clownModels[24] = loadModel("assets/models/clown_pinkyellowdot.gltf");

		clownBoingSound = getSoundID(*loadSound("assets/sounds/clownboing.wav"));

		clownMixes[0 * 1000 + 0] = 0;
		clownMixes[0 * 1000 + 1] = 5;
		clownMixes[0 * 1000 + 2] = 6;
		clownMixes[0 * 1000 + 3] = 7;
		clownMixes[0 * 1000 + 4] = 8;
		clownMixes[1 * 1000 + 0] = 9;
		clownMixes[1 * 1000 + 1] = 1;
		clownMixes[1 * 1000 + 2] = 10;
		clownMixes[1 * 1000 + 3] = 11;
		clownMixes[1 * 1000 + 4] = 12;
		clownMixes[2 * 1000 + 0] = 13;
		clownMixes[2 * 1000 + 1] = 14;
		clownMixes[2 * 1000 + 2] = 2;
		clownMixes[2 * 1000 + 3] = 15;
		clownMixes[2 * 1000 + 4] = 16;
		clownMixes[3 * 1000 + 0] = 17;
		clownMixes[3 * 1000 + 1] = 18;
		clownMixes[3 * 1000 + 2] = 19;
		clownMixes[3 * 1000 + 3] = 3;
		clownMixes[3 * 1000 + 4] = 20;
		clownMixes[4 * 1000 + 0] = 21;
		clownMixes[4 * 1000 + 1] = 22;
		clownMixes[4 * 1000 + 2] = 23;
		clownMixes[4 * 1000 + 3] = 24;
		clownMixes[4 * 1000 + 4] = 4;

		std::uniform_int_distribution<int> randomMixedClownColor(0, 4);
		wantedClownColor = randomMixedClownColor(rand);
		wantedClownDot = randomMixedClownColor(rand);
	}

	std::string clownColorToString(int color) {
		switch (color) {
		case 0:
			return "Red";
		case 1:
			return "Blue";
		case 2:
			return "Green";
		case 3:
			return "Yellow";
		case 4:
			return "Pink";
		default:
			return "C\'est ciao.";
		}
	}

	void update(double dt) {
		const Transform& cameraTransform = getEntityComponent<Transform>(camera);

		std::uniform_real_distribution<float> distM10To10(-10.0f, 10.0f);
		std::uniform_real_distribution<float> dist0P5To2P5(0.5f, 2.0f);

		const Math::vec3 cameraButtonRed = buttonRedPosition - cameraTransform.position;
		const Math::vec3 cameraButtonBlue = buttonBluePosition - cameraTransform.position;
		const Math::vec3 cameraButtonGreen = buttonGreenPosition - cameraTransform.position;
		const Math::vec3 cameraButtonYellow = buttonYellowPosition - cameraTransform.position;
		const Math::vec3 cameraButtonPink = buttonPinkPosition - cameraTransform.position;

		if (Math::dot(cameraButtonRed, cameraButtonRed) < 25.0f) {
			if (!onButtonRed) {
				clowns.insert(createClown(0, 0, Math::vec3(distM10To10(rand), 6.0f, distM10To10(rand)), dist0P5To2P5(rand)));
				onButtonRed = true;
			}
			getEntityComponent<Transform>(buttonRed).position.y = buttonRedPosition.y - 0.25f;
		}
		else {
			onButtonRed = false;
			getEntityComponent<Transform>(buttonRed).position.y = buttonRedPosition.y;
		}

		if (Math::dot(cameraButtonBlue, cameraButtonBlue) < 25.0f) {
			if (!onButtonBlue) {
				clowns.insert(createClown(1, 1, Math::vec3(distM10To10(rand), 6.0f, distM10To10(rand)), dist0P5To2P5(rand)));
				onButtonBlue = true;
			}
			getEntityComponent<Transform>(buttonBlue).position.y = buttonBluePosition.y - 0.25f;
		}
		else {
			onButtonBlue = false;
			getEntityComponent<Transform>(buttonBlue).position.y = buttonBluePosition.y;
		}

		if (Math::dot(cameraButtonGreen, cameraButtonGreen) < 25.0f) {
			if (!onButtonGreen) {
				clowns.insert(createClown(2, 2, Math::vec3(distM10To10(rand), 6.0f, distM10To10(rand)), dist0P5To2P5(rand)));
				onButtonGreen = true;
			}
			getEntityComponent<Transform>(buttonGreen).position.y = buttonGreenPosition.y - 0.25f;
		}
		else {
			onButtonGreen = false;
			getEntityComponent<Transform>(buttonGreen).position.y = buttonGreenPosition.y;
		}

		if (Math::dot(cameraButtonYellow, cameraButtonYellow) < 25.0f) {
			if (!onButtonYellow) {
				clowns.insert(createClown(3, 3, Math::vec3(distM10To10(rand), 6.0f, distM10To10(rand)), dist0P5To2P5(rand)));
				onButtonYellow = true;
			}
			getEntityComponent<Transform>(buttonYellow).position.y = buttonYellowPosition.y - 0.25f;
		}
		else {
			onButtonYellow = false;
			getEntityComponent<Transform>(buttonYellow).position.y = buttonYellowPosition.y;
		}

		if (Math::dot(cameraButtonPink, cameraButtonPink) < 25.0f) {
			if (!onButtonPink) {
				clowns.insert(createClown(4, 4, Math::vec3(distM10To10(rand), 6.0f, distM10To10(rand)), dist0P5To2P5(rand)));
				onButtonPink = true;
			}
			getEntityComponent<Transform>(buttonPink).position.y = buttonPinkPosition.y - 0.25f;
		}
		else {
			onButtonPink = false;
			getEntityComponent<Transform>(buttonPink).position.y = buttonPinkPosition.y;
		}

		for (auto it = clowns.begin(); it != clowns.end(); ) {
			if (!entityExists(*it)) {
				it = clowns.erase(it);
				continue;
			}

			const Math::vec3 clownPosition = getEntityComponent<Transform>(*it).position;
			if (clownPosition.z > 30.0f) {
				Scriptable& clownScriptable = getEntityComponent<Scriptable>(*it);
				ClownScript* clownScript = static_cast<ClownScript*>(clownScriptable.script);
				if (clownScript->type == clownMixes[wantedClownColor * 1000 + wantedClownDot]) {
					std::uniform_int_distribution<int> randomMixedClownColor(0, 4);
					wantedClownColor = randomMixedClownColor(rand);
					wantedClownDot = randomMixedClownColor(rand);
					point++;
					destroyEntity(*it);
					it = clowns.erase(it);
					continue;
				}
			}

			if (Math::dot(clownPosition - cameraTransform.position, clownPosition - cameraTransform.position) > 10000.0f) {
				destroyEntity(*it);
				it = clowns.erase(it);
				continue;
			}

			if (clownColorLeft == -1) {
				const float clownToBolLeftSqDist = Math::dot(bolLeftPos - clownPosition, bolLeftPos - clownPosition);
				if (clownToBolLeftSqDist < 18.0f) {
					Scriptable& clownScriptable = getEntityComponent<Scriptable>(*it);
					ClownScript* clownScript = static_cast<ClownScript*>(clownScriptable.script);
					clownColorLeft = clownScript->color;
					destroyEntity(signLeft);
					signLeft = createSign(0, clownColorLeft);
					destroyEntity(*it);
					it = clowns.erase(it);
					continue;
				}
			}

			if (clownColorRight == -1) {
				const float clownToBolRightSqDist = Math::dot(bolRightPos - clownPosition, bolRightPos - clownPosition);
				if (clownToBolRightSqDist < 18.0f) {
					Scriptable& clownScriptable = getEntityComponent<Scriptable>(*it);
					ClownScript* clownScript = static_cast<ClownScript*>(clownScriptable.script);
					clownColorRight = clownScript->color;
					destroyEntity(signRight);
					signRight = createSign(1, clownColorRight);
					destroyEntity(*it);
					it = clowns.erase(it);
					continue;
				}
			}

			if ((clownColorLeft != -1) && (clownColorRight != -1)) {
				break;
			}

			it++;
		}

		if ((clownColorLeft != -1) && (clownColorRight != -1)) {
			clowns.insert(createClown(clownMixes[clownColorLeft * 1000 + clownColorRight], clownColorLeft, clownMixSpawn, dist0P5To2P5(rand)));
			clownColorLeft = -1;
			clownColorRight = -1;
			destroyEntity(signLeft);
			destroyEntity(signRight);
			signLeft = createSign(0, -1);
			signRight = createSign(1, -1);

		}

		time += dt;
		if (time > 300000.0) {
			drawUIText(fontBig, "FINISHED", Math::vec2((float)getWindowWidth() / 2.0f - 150.0f, (float)getWindowHeight() / 2.0f), Math::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			drawUIText(font, "Score: " + std::to_string(point), Math::vec2((float)getWindowWidth() / 2.0f, (float)getWindowHeight() / 2.0f + 100.0f), Math::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			if (time > 302000.0) {
				closeWindow();
				return;
			}
		}
		else {
			drawUIText(font, "Laughs: " + std::to_string(point), Math::vec2(20.0f, (float)getWindowHeight() - 20.0f), Math::vec4(1.0f, 1.0f, 1.0f, 1.0f));

			if (wantedClownColor == wantedClownDot) {
				drawUIText(font, "I need a " + clownColorToString(wantedClownColor) + " clown with White dots.", Math::vec2(20.0f, 50.0f), Math::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			}
			else {
				drawUIText(font, "I need a " + clownColorToString(wantedClownColor) + " clown with " + clownColorToString(wantedClownDot) + " dots.", Math::vec2(20.0f, 50.0f), Math::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			}
		}



		/*if (time >= 1000.0) {
			std::uniform_real_distribution<float> distM10To10(-10.0f, 10.0f);
			std::uniform_real_distribution<float> dist0P5To2P5(0.5f, 2.0f);
			std::uniform_int_distribution<int> model0To4(0, 4);

			int clownColor = model0To4(rand);
			clowns.insert(createClown(clownColor, clownColor, Math::vec3(distM10To10(rand), 6.0f, distM10To10(rand)), dist0P5To2P5(rand)));

			time -= 1000.0;
		}*/
	}

	void destroy() {
	}

private:
	FontID font;
	FontID fontBig;

	Entity camera;

	Entity buttonRed;
	Math::vec3 buttonRedPosition;

	Entity buttonBlue;
	Math::vec3 buttonBluePosition;

	Entity buttonGreen;
	Math::vec3 buttonGreenPosition;

	Entity buttonYellow;
	Math::vec3 buttonYellowPosition;

	Entity buttonPink;
	Math::vec3 buttonPinkPosition;

	Entity signLeft;
	Math::vec3 signLeftPosition;

	Entity signRight;
	Math::vec3 signRightPosition;

	Model* signRobeNormal;
	Model* signRobeRed;
	Model* signRobeBlue;
	Model* signRobeGreen;
	Model* signRobeYellow;
	Model* signRobePink;

	Model* signDotNormal;
	Model* signDotRed;
	Model* signDotBlue;
	Model* signDotGreen;
	Model* signDotYellow;
	Model* signDotPink;

	Math::vec3 bolLeftPos;
	Math::vec3 bolRightPos;
	Math::vec3 clownMixSpawn;

	bool onButtonRed = false;
	bool onButtonBlue = false;
	bool onButtonGreen = false;
	bool onButtonYellow = false;
	bool onButtonPink = false;

	Bimap<int, Model*> clownModels;
	std::unordered_map<int, int> clownMixes;

	SoundID clownBoingSound;

	double time = 0.0;
	std::default_random_engine rand;

	std::set<Entity> clowns;

	int clownColorLeft = -1;
	int clownColorRight = -1;

	int wantedClownColor = -1;
	int wantedClownDot = -1;

	int point = 0;
};