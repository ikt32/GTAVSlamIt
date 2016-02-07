#include "script.h"
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include "VehicleExtensions.hpp"

enum ControlType {
	Button = 0,
	SIZE_OF_ARRAY
};

Vehicle vehicle;
VehExt::VehicleExtensions ext;
Player player;
Ped playerPed;

int controls[SIZE_OF_ARRAY];
bool controlCurr[SIZE_OF_ARRAY];
bool controlPrev[SIZE_OF_ARRAY];

int slamLevel;

void readSettings() {
	controls[Button] = GetPrivateProfileInt(L"MAIN", L"SwitchSlam", VK_DOWN, L"./SlamIt.ini");
}

bool isKeyPressed(int key) {
	if (GetAsyncKeyState(key) & 0x8000)
		return true;
	return false;
}

bool isKeyJustPressed(int key, ControlType control) {
	if (GetAsyncKeyState(key) & 0x8000)
		controlCurr[control] = true;
	else
		controlCurr[control] = false;

	// raising edge
	if (controlCurr[control] == true && controlPrev[control] == false) {
		controlPrev[control] = controlCurr[control];
		return true;
	}

	controlPrev[control] = controlCurr[control];
	return false;
}

void showText(float x, float y, float scale, char * text) {
	UI::SET_TEXT_FONT(0);
	UI::SET_TEXT_SCALE(scale, scale);
	UI::SET_TEXT_COLOUR(255, 255, 255, 255);
	UI::SET_TEXT_WRAP(0.0, 1.0);
	UI::SET_TEXT_CENTRE(0);
	UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
	UI::SET_TEXT_EDGE(1, 0, 0, 0, 205);
	UI::_SET_TEXT_ENTRY("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING(text);
	UI::_DRAW_TEXT(x, y);
}

void showNotification(char *message) {
	UI::_SET_NOTIFICATION_TEXT_ENTRY("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING(message);
	UI::_DRAW_NOTIFICATION(false, false);
}

void slam(Vehicle vehicle, int slamLevel) {
	if (ENTITY::DOES_ENTITY_EXIST(vehicle)){
		switch (slamLevel) {
		case (2) :
			ext.SetWheelsHealth(vehicle, 0.0f);
			break;
		case (1) :
			ext.SetWheelsHealth(vehicle, 400.0f);
			break;
		default:
		case (0) :
			ext.SetWheelsHealth(vehicle, 1000.0f);
			break;
		}
	}
}

void update() {
	player = PLAYER::PLAYER_ID();
	playerPed = PLAYER::PLAYER_PED_ID();

	// check if player ped exists and control is on (e.g. not in a cutscene)
	if (!ENTITY::DOES_ENTITY_EXIST(playerPed) || !PLAYER::IS_PLAYER_CONTROL_ON(player))
		return;

	// check for player ped death and player arrest
	if (ENTITY::IS_ENTITY_DEAD(playerPed) || PLAYER::IS_PLAYER_BEING_ARRESTED(player, TRUE))
		return;

	vehicle = PED::GET_VEHICLE_PED_IS_IN(playerPed, false);

	if (!ENTITY::DOES_ENTITY_EXIST(vehicle))
		return;

	Hash model = ENTITY::GET_ENTITY_MODEL(vehicle);

	if (!VEHICLE::IS_THIS_MODEL_A_CAR(model)
	    || VEHICLE::IS_THIS_MODEL_A_BIKE(model)
		|| VEHICLE::IS_THIS_MODEL_A_QUADBIKE(model)
		|| VEHICLE::IS_THIS_MODEL_A_BICYCLE(model))
		return;

	if (isKeyJustPressed(controls[Button], Button)) {
		readSettings();
		std::stringstream message;
		slamLevel++;
		if (slamLevel > 2) {
			slamLevel = 0;
		}
		slam(vehicle, slamLevel);
		message << "Slam level: " << slamLevel;
		showNotification((char *)message.str().c_str());
	}
}

void main() {
	readSettings();
	while (true) {
		update();
		WAIT(0);
	}
}

void ScriptMain() {
	srand(GetTickCount());
	main();
}
