#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "glm/glm.hpp"
#include "SDL2/SDL_scancode.h"
#include "SDL2/SDL_gamecontroller.h"
#include "SDL2/SDL_events.h"

#include <vector>
#include <filesystem>

#define MAX_KEYS 256
#define MAX_MOUSE_BUTTONS 5
#define MAX_PADS 4
#define SCREEN_SIZE 1

struct _SDL_GameController;
struct _SDL_Haptic;

namespace fs = std::filesystem;

enum KeyState
{
	KEY_IDLE,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

struct GamePad
{
	bool start, back, guide;
	bool x, y, a, b, l1, r1, l3, r3;
	bool up, down, left, right;
	float l2, r2;
	float left_x, left_y, right_x, right_y, left_dz, right_dz;

	bool enabled;
	int index;
	_SDL_GameController* controller;

	KeyState buttonStates[SDL_CONTROLLER_BUTTON_MAX];
	KeyState previousButtonStates[SDL_CONTROLLER_BUTTON_MAX];
};

class InputManager
{
public:
	InputManager();
	~InputManager();

	bool Init();
	bool PreUpdate();
	bool CleanUp();

	bool ProcessSDLEvents();

	// Called at PreUpdate
	void UpdateGamepadsInput();

	void HandleDeviceConnection(int index);
	void HandleDeviceRemoval(int index);

	KeyState GetGamepadButton(int gamepadId, int button) const;

	const Uint8* GetKeyboardState(int* numkeys) { return SDL_GetKeyboardState(numkeys); }
	KeyState GetKey(int id) const { return keyboard[id]; }

	KeyState GetMouseButton(int id) const { return mouse_buttons[id]; }

	int GetMouseX() { return mouse_x; }
	int GetMouseY() { return mouse_y; }
	int GetMouseZ() { return mouse_z; }

	int GetMouseXMotion() const { return mouse_x_motion; }
	int GetMouseYMotion() const { return mouse_y_motion; }

	std::vector<SDL_Event> GetSDLEvents() { return events; }
	std::string GetDropFile() { return fileDir; }
	glm::vec2 GetWindowSize() { return windowSize; }

	bool IsFileDropped() { return dropFile; }
	bool IsWindowResized() { return windowResize; }
	bool GetEngineShutDown() { return shutDownEngine; }
	void EngineShutDown() { shutDownEngine = true; }

public:
	// Pad
	GamePad pads[MAX_PADS];

private:

	// Keyboard
	KeyState keyboard[MAX_KEYS];

	// Mouse
	KeyState mouse_buttons[MAX_MOUSE_BUTTONS];
	int mouse_x;
	int mouse_y;
	int mouse_z;
	int mouse_x_motion;
	int mouse_y_motion;
	float dx;
	float dy;

	// Store Data
	std::vector<SDL_Event> events;
	std::string fileDir;
	glm::vec2 windowSize;

	// Flags
	bool dropFile = false;
	bool windowResize = false;
	bool shutDownEngine = false;
};

#endif //INPUT_MANAGER_H
