#include <GL/glew.h>
#include <chrono>
#include <thread>
#include <exception>
#include <glm/glm.hpp>
#include "MyWindow.h"
#include "MyGUI.h"
#include <iostream>
#include <fstream>
#include "Camera.h"
#include "Texture.h"
#include "Mesh.h"
#include "GraphicObject.h"

using namespace std;

using hrclock = chrono::high_resolution_clock;
using u8vec4 = glm::u8vec4;
using ivec2 = glm::ivec2;
using vec3 = glm::dvec3;

static const ivec2 WINDOW_SIZE(1280, 720);
static const auto FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;

static bool paused = true;
static Camera camera;
static GraphicObject scene;

static void drawAxis(double size) {
	glLineWidth(2.0);
	glBegin(GL_LINES);
	glColor3ub(255, 0, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(size, 0, 0);
	glColor3ub(0, 255, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, size, 0);
	glColor3ub(0, 0, 255);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, size);
	glEnd();
}

inline static void glVertex3(const vec3& v) { glVertex3dv(&v.x); }
static void drawWiredQuad(const vec3& v0, const vec3& v1, const vec3& v2, const vec3& v3) {
	glBegin(GL_LINE_LOOP);
	glVertex3(v0);
	glVertex3(v1);
	glVertex3(v2);
	glVertex3(v3);
	glEnd();
}

static void drawBoundingBox(const BoundingBox& bbox) {
	glLineWidth(2.0);
	drawWiredQuad(bbox.v000(), bbox.v001(), bbox.v011(), bbox.v010());
	drawWiredQuad(bbox.v100(), bbox.v101(), bbox.v111(), bbox.v110());
	drawWiredQuad(bbox.v000(), bbox.v001(), bbox.v101(), bbox.v100());
	drawWiredQuad(bbox.v010(), bbox.v011(), bbox.v111(), bbox.v110());
	drawWiredQuad(bbox.v000(), bbox.v010(), bbox.v110(), bbox.v100());
	drawWiredQuad(bbox.v001(), bbox.v011(), bbox.v111(), bbox.v101());

}

static auto MakeTriangleMesh(double size) {
	const glm::vec3 vertices[] = { glm::vec3(-size, -size, 0), glm::vec3(size, -size, 0), glm::vec3(0, size, 0) };
	const unsigned int indices[] = { 0, 1, 2 };
	const glm::vec2 texcoords[] = { glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(0.5, 0) };
	const glm::vec3 normals[] = { glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1) };
	const glm::u8vec3 colors[] = { glm::u8vec3(255, 0, 0), glm::u8vec3(0, 255, 0), glm::u8vec3(0, 0, 255) };

	auto mesh_ptr = make_shared<Mesh>();
	mesh_ptr->load(vertices, 3, indices, 3);
	mesh_ptr->loadTexCoords(texcoords);
	mesh_ptr->loadNormals(normals);
	mesh_ptr->loadColors(colors);
	return mesh_ptr;
}

static auto MakeQuadMesh(double size) {
	const glm::vec3 vertices[4] = { glm::vec3(-size, -size, 0), glm::vec3(size, -size, 0), glm::vec3(size, size, 0), glm::vec3(-size, size, 0) };
	const unsigned int indices[6] = { 0, 1, 2, 2, 3, 0 };
	const glm::vec2 texcoords[4] = { glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(1, 0), glm::vec2(0,0) };
	const glm::vec3 normals[4] = { glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0,0,1) };
	const glm::u8vec3 colors[4] = { glm::u8vec3(255, 0, 0), glm::u8vec3(0, 255, 0), glm::u8vec3(0, 0, 255), glm::u8vec3(255,255,0) };

	auto mesh_ptr = make_shared<Mesh>();
	mesh_ptr->load(vertices, 4, indices, 6);
	mesh_ptr->loadTexCoords(texcoords);
	mesh_ptr->loadNormals(normals);
	mesh_ptr->loadColors(colors);
	return mesh_ptr;
}

static auto MakeChessTextureImage(int width, int height, int quad_size) {
	auto image_ptr = make_shared<Image>();

	ifstream file("chess_texture.tex", ios::binary);
	if (file) file >> (*image_ptr);
	else {
		const glm::u8vec3 white = glm::u8vec3(255, 255, 255);
		const glm::u8vec3 black = glm::u8vec3(0, 0, 0);

		vector<glm::u8vec3> colors(width * height);
		for (int y = 0; y < height; ++y) {
			int y_quad = y / quad_size;
			for (int x = 0; x < width; ++x) {
				int x_quad = x / quad_size;
				colors[y * width + x] = (x_quad + y_quad) % 2 ? white : black;
			}
		}
		image_ptr->load(width, height, 3, colors.data());
		ofstream file("chess_texture.tex", ios::binary);
		if (file) file << (*image_ptr);
	}
	return image_ptr;
}

static void drawFloorGrid(int size, double step) {
	glColor3ub(0, 0, 0);
	glBegin(GL_LINES);
	for (double i = -size; i <= size; i += step) {
		glVertex3d(i, 0, -size);
		glVertex3d(i, 0, size);
		glVertex3d(-size, 0, i);
		glVertex3d(size, 0, i);
	}
	glEnd();
}

static void drawDebugInfoForGraphicObject(const GraphicObject& obj) {
	glPushMatrix();
	glColor3ub(255, 255, 0);
	drawBoundingBox(obj.boundingBox());
	glMultMatrixd(obj.transform().data());
	drawAxis(0.5);
	glColor3ub(0, 255, 255);
	drawBoundingBox(obj.localBoundingBox());

	glColor3ub(255, 0, 0);
	if (obj.hasMesh()) drawBoundingBox(obj.mesh().boundingBox());

	for (const auto& child : obj.children()) drawDebugInfoForGraphicObject(child);
	glPopMatrix();
}

static void drawWorldDebugInfoForGraphicObject(const GraphicObject& obj) {
	glColor3ub(255, 255, 255);
	drawBoundingBox(obj.worldBoundingBox());
	for (const auto& child : obj.children()) drawWorldDebugInfoForGraphicObject(child);
}

static void display_func() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(&camera.view()[0][0]);

	drawFloorGrid(16, 0.25);
	scene.draw();

	glColor3ub(255, 255, 255);
	drawDebugInfoForGraphicObject(scene);
	drawWorldDebugInfoForGraphicObject(scene);

	//glutSwapBuffers();
}

static void init_opengl() {
	glewInit();

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glClearColor(0.5, 0.5, 0.5, 1.0);
}

static void reshape_func(int width, int height) {
	glViewport(0, 0, width, height);
	camera.aspect = static_cast<double>(width) / height;
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(&camera.projection()[0][0]);
}

const double moveSpeed = 0.1;
const float mouseSensitivity = 0.005f;
static int lastMouseX, lastMouseY;
static bool firstMouse = true;

static void camera_func(Transform& cameraTransform, const Uint8* keyState, int mouseX, int mouseY, bool rightClickHeld) {

	if (rightClickHeld) {
		if (firstMouse) {
			lastMouseX = mouseX;
			lastMouseY = mouseY;
			firstMouse = false;
		}

		// Calcula el delta del ratón
		float deltaX = (mouseX - lastMouseX) * mouseSensitivity;
		float deltaY = (mouseY - lastMouseY) * mouseSensitivity;

		// Actualiza el último estado del ratón
		lastMouseX = mouseX;
		lastMouseY = mouseY;

		// Rota la cámara con los desplazamientos de yaw y pitch
		cameraTransform.rotateYawPitch(-deltaX, deltaY);

		// Movimiento de la cámara con WASD
		if (keyState[SDL_SCANCODE_W]) cameraTransform.translate(cameraTransform.fwd() * -moveSpeed);
		if (keyState[SDL_SCANCODE_S]) cameraTransform.translate(cameraTransform.fwd() * moveSpeed);
		if (keyState[SDL_SCANCODE_A]) cameraTransform.translate(cameraTransform.left() * -moveSpeed);
		if (keyState[SDL_SCANCODE_D]) cameraTransform.translate(cameraTransform.left() * moveSpeed);
	}
	else {
		firstMouse = true;  // Reinicia el primer movimiento del ratón al soltar el clic derecho
	}
}

static void idle_func() {
	if (!paused) {
		//animate triangles
		scene.children().front().transform().rotate(0.0001, vec3(0, 0, 1));
		scene.children().front().children().front().transform().rotate(0.0001, vec3(0, 0, 1));
		scene.children().front().children().front().children().front().transform().rotate(0.0001, vec3(0, 0, 1));
	}
}

static void initScene() {
	auto& triangle = scene.emplaceChild();
	triangle.transform().pos() = vec3(0, 0, 0);
	triangle.color() = glm::u8vec3(255, 0, 0);

	auto& child_textured_triangle = triangle.emplaceChild();
	child_textured_triangle.transform().pos() = vec3(2, 0, 0);
	child_textured_triangle.color() = glm::u8vec3(0, 255, 0);

	auto& child_textured_quad = child_textured_triangle.emplaceChild();
	child_textured_quad.transform().pos() = vec3(2, 0, 0);
	child_textured_quad.color() = glm::u8vec3(0, 0, 255);

	auto triangle_mesh = MakeTriangleMesh(0.5);
	auto quad_mesh = MakeQuadMesh(0.5);
	auto chess_texture_image = MakeChessTextureImage(64, 64, 8);

	triangle.setMesh(triangle_mesh);
	child_textured_triangle.setMesh(triangle_mesh);
	child_textured_triangle.setTextureImage(chess_texture_image);
	child_textured_quad.setMesh(quad_mesh);
	child_textured_quad.setTextureImage(chess_texture_image);
}

int main(int argc, char** argv) {
	MyWindow window("Sofo Engine", WINDOW_SIZE.x, WINDOW_SIZE.y);
	MyGUI gui(window.windowPtr(), window.contextPtr());

	init_opengl();

	// Init camera
	camera.transform().pos() = vec3(0, 1, 4);
	camera.transform().rotate(glm::radians(180.0), vec3(0, 1, 0));

	initScene();

	bool running = true;
	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	SDL_Event e;

	while (running) {
		const auto t0 = hrclock::now();
		display_func();
		idle_func();
		reshape_func(WINDOW_SIZE.x, WINDOW_SIZE.y);
		gui.render();
		window.swapBuffers();
		const auto t1 = hrclock::now();
		const auto dt = t1 - t0;
		if (dt < FRAME_DT) this_thread::sleep_for(FRAME_DT - dt);

		int mouseX, mouseY;
		Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);

		while (SDL_PollEvent(&e))
		{
			gui.processEvent(e);

			switch (e.type) {
			case SDL_QUIT:
				running = false;
				break;
			//case SDL_DROPFILE:
			//	dropped_filePath = e.drop.file;
			//	extension = getFileExtension(dropped_filePath);

			//	if (extension == "obj" || extension == "fbx" || extension == "dae") {
			//		mesh->LoadFile(dropped_filePath);
			//		GameObject go;
			//		go.AddComponent<MeshLoader>()->SetMesh(mesh);
			//		go.setMesh(mesh);
			//		scene.emplaceChild(go);
			//	}
			//	else if (extension == "png" || extension == "jpg" || extension == "bmp") {
			//		int mouseX, mouseY;
			//		SDL_GetMouseState(&mouseX, &mouseY);
			//		for (auto& child : scene.children()) {
			//			if (isMouseOverGameObject(child, mouseX, mouseY)) {
			//				imageTexture->LoadTexture(dropped_filePath);
			//				texture->setImage(imageTexture);
			//				child.GetComponent<MeshLoader>()->GetMesh()->deleteCheckerTexture();
			//				child.GetComponent<MeshLoader>()->SetImage(imageTexture);
			//				child.GetComponent<MeshLoader>()->SetTexture(texture);
			//			}
			//		}

			//	}
			//	else {
			//		std::cerr << "Unsupported file extension: " << extension << std::endl;
			//	}
			//	SDL_free(dropped_filePath);
			//	//Hasta aquí
			//	break;
			}
		}

		camera_func(camera.transform(), keyState, mouseX, mouseY, mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT));
	}

	return 0;
}