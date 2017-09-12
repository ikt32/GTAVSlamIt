#include "script.h"
#include "keyboard.h"
#include <sstream>
#include <iomanip>
#include "../../GTAVManualTransmission/Gears/Memory/VehicleExtensions.hpp"

enum ControlType {
	Button = 0,
	SIZE_OF_ARRAY
};

Vehicle vehicle;
Vehicle prevVehicle;
VehicleExtensions ext;
Player player;
Ped playerPed;

int controls[SIZE_OF_ARRAY];
bool controlCurr[SIZE_OF_ARRAY];
bool controlPrev[SIZE_OF_ARRAY];

int slamLevel = 0;
int prevNotification = 0;

void readSettings() {
	char kbKeyBuffer[24];
	GetPrivateProfileStringA("MAIN", "SwitchSlam", "DOWN", kbKeyBuffer, 24, "./SlamIt.ini");
	controls[Button] = str2key(kbKeyBuffer);
}

bool isKeyPressed(int key) {
	if (IsKeyDown(key))
		return true;
	return false;
}

bool isKeyJustPressed(int key, ControlType control) {
	if (IsKeyDown(key))
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

void showText(float x, float y, float scale, const char * text) {
	UI::SET_TEXT_FONT(0);
	UI::SET_TEXT_SCALE(scale, scale);
	UI::SET_TEXT_COLOUR(255, 255, 255, 255);
	UI::SET_TEXT_WRAP(0.0, 1.0);
	UI::SET_TEXT_CENTRE(0);
	UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
	UI::SET_TEXT_EDGE(1, 0, 0, 0, 205);
	UI::BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
	UI::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(const_cast<char *>(text));
	UI::END_TEXT_COMMAND_DISPLAY_TEXT(x, y);
}

void showNotification(const char * message) {
	if (prevNotification)
		UI::_REMOVE_NOTIFICATION(prevNotification);
	UI::_SET_NOTIFICATION_TEXT_ENTRY("STRING");
	UI::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(const_cast<char *>(message));
	prevNotification = UI::_DRAW_NOTIFICATION(false, false);
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

	if (prevVehicle != vehicle) {
		slamLevel = 0;
	}
	prevVehicle = vehicle;

	if (isKeyJustPressed(controls[Button], Button)) {
		readSettings();
		std::stringstream message;
		slamLevel++;
		if (slamLevel > 2) {
			slamLevel = 0;
		}
		slam(vehicle, slamLevel);
		message << "Slam level: " << slamLevel;
		showNotification(message.str().c_str());
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
