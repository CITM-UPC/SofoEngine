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
#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include <IL/il.h>
#include "Image.h"
#include <locale>
#include <codecvt>
#include <IL/ilu.h>
#include <IL/ilut.h>
#include <filesystem>
#include "Scene.h"
#include <glm/gtc/type_ptr.hpp>
#include <random>

using namespace std;

using hrclock = chrono::high_resolution_clock;
using u8vec4 = glm::u8vec4;
using ivec2 = glm::ivec2;
using vec3 = glm::dvec3;

static const ivec2 WINDOW_SIZE(1280, 720);
static const auto FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;


static std::string generateRandomNameFromBase(const std::string& baseName, size_t numDigits = 4) {
	std::default_random_engine rng(std::random_device{}());
	std::uniform_int_distribution<> dist(0, 9);

	std::string randomName = baseName + "_";
	for (size_t i = 0; i < numDigits; ++i) {
		randomName += std::to_string(dist(rng));
	}
	return randomName;
}

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

static std::vector<std::shared_ptr<Mesh>> loadMeshesFromFile(const std::string& filePath) {
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);

	if (!scene || !scene->HasMeshes()) {
		throw std::runtime_error("Issue loading the model: " + filePath);
	}

	std::vector<std::shared_ptr<Mesh>> meshes;

	for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		const aiMesh* aiMesh = scene->mMeshes[meshIndex];

		std::vector<glm::vec3> vertices(aiMesh->mNumVertices);
		for (size_t i = 0; i < aiMesh->mNumVertices; ++i) {
			vertices[i] = glm::vec3(aiMesh->mVertices[i].x, aiMesh->mVertices[i].y, aiMesh->mVertices[i].z);
		}

		std::vector<unsigned int> indices;
		for (size_t i = 0; i < aiMesh->mNumFaces; ++i) {
			const aiFace& face = aiMesh->mFaces[i];
			for (size_t j = 0; j < face.mNumIndices; ++j) {
				indices.push_back(face.mIndices[j]);
			}
		}

		std::vector<glm::vec2> texCoords;
		if (aiMesh->HasTextureCoords(0)) {
			texCoords.resize(aiMesh->mNumVertices);
			for (size_t i = 0; i < aiMesh->mNumVertices; ++i) {
				texCoords[i] = glm::vec2(aiMesh->mTextureCoords[0][i].x, aiMesh->mTextureCoords[0][i].y);
			}
		}

		std::vector<glm::vec3> normals;
		if (aiMesh->HasNormals()) {
			normals.resize(aiMesh->mNumVertices);
			for (size_t i = 0; i < aiMesh->mNumVertices; ++i) {
				normals[i] = glm::vec3(aiMesh->mNormals[i].x, aiMesh->mNormals[i].y, aiMesh->mNormals[i].z);
			}
		}

		std::vector<glm::u8vec3> colors;
		if (aiMesh->HasVertexColors(0)) {
			colors.resize(aiMesh->mNumVertices);
			for (size_t i = 0; i < aiMesh->mNumVertices; ++i) {
				colors[i] = glm::u8vec3(
					static_cast<unsigned char>(aiMesh->mColors[0][i].r * 255),
					static_cast<unsigned char>(aiMesh->mColors[0][i].g * 255),
					static_cast<unsigned char>(aiMesh->mColors[0][i].b * 255)
				);
			}
		}

		auto mesh = std::make_shared<Mesh>();
		mesh->load(vertices.data(), vertices.size(), indices.data(), indices.size());

		// Basic Data
		std::string standarizedName = aiMesh->mName.C_Str();
		size_t index = 0;
		while ((index = standarizedName.find('.', index)) != std::string::npos) {
			standarizedName.replace(index, 1, "_");
			index++;
		}
		mesh->setName(standarizedName);

		glm::mat4 mTransform(1);
		if (aiMesh->mName != (aiString)"Scene")
		{
			aiNode* meshNode = scene->mRootNode->FindNode(aiMesh->mName);
			if (meshNode)
			{
				aiMatrix4x4 transform = meshNode->mTransformation;
				mTransform = glm::transpose(glm::make_mat4(&transform.a1));
			}
		}
		mesh->loadModelMatrix(mTransform);

		if (!texCoords.empty()) {
			mesh->loadTexCoords(texCoords.data());
		}

		if (!normals.empty()) {
			mesh->loadNormals(normals.data());
		}

		if (!colors.empty()) {
			mesh->loadColors(colors.data());
		}

		meshes.push_back(mesh);
	}

	return meshes;
}

static std::string wstringToString(const wchar_t* wstr) {
	if (!wstr) return "Unknown error";

	size_t len = wcslen(wstr);
	size_t converted = 0;
	std::string result(len, '\0');

	wcstombs_s(&converted, &result[0], len + 1, wstr, len);

	return result.empty() ? "Unknown error" : result;
}

static std::shared_ptr<Image> loadImageFromFile(const std::string& filePath) {
	static bool isDevILInitialized = false;
	if (!isDevILInitialized) {
		ilInit();
		isDevILInitialized = true;
	}

	ILuint imageID;
	ilGenImages(1, &imageID);
	ilBindImage(imageID);

	if (!ilLoadImage((const wchar_t*)filePath.c_str())) {
		ILenum error = ilGetError();
		ilDeleteImages(1, &imageID);

		const wchar_t* errorStr = iluErrorString(error);
		std::string errorMessage = wstringToString(errorStr);

		if (errorMessage.empty()) {
			errorMessage = "Unknown error while loading the image.";
		}

		std::cout << "Error while loading the image: " << filePath << "\nError details: " << errorMessage << std::endl;
	}

	int width = ilGetInteger(IL_IMAGE_WIDTH);
	int height = ilGetInteger(IL_IMAGE_HEIGHT);
	int channels = ilGetInteger(IL_IMAGE_CHANNELS);
	ILenum format = (channels == 4) ? IL_RGBA : IL_RGB;
	std::cout << "Image loaded width: " << width << " Height: " << height << " Channels: " << channels << std::endl;

	if (!ilConvertImage(format, IL_UNSIGNED_BYTE)) {
		ilDeleteImages(1, &imageID);
		throw std::runtime_error("Error converting the image: " + filePath);
	}

	void* data = ilGetData();

	std::shared_ptr<Image> image = std::make_shared<Image>();
	image->load(width, height, channels, data);

	ilDeleteImages(1, &imageID);

	return image;
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
	glLoadMatrixd(&Scene::get().camera.view()[0][0]);

	drawFloorGrid(16, 0.25);
	Scene::get().scene.draw();

	glColor3ub(255, 255, 255);
	drawDebugInfoForGraphicObject(Scene::get().scene);
	drawWorldDebugInfoForGraphicObject(Scene::get().scene);

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
	Scene::get().camera.aspect = static_cast<double>(width) / height;
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(&Scene::get().camera.projection()[0][0]);
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

		float deltaX = (mouseX - lastMouseX) * mouseSensitivity;
		float deltaY = (mouseY - lastMouseY) * mouseSensitivity;

		lastMouseX = mouseX;
		lastMouseY = mouseY;

		cameraTransform.rotateYawPitch(-deltaX, deltaY);

		if (keyState[SDL_SCANCODE_W]) cameraTransform.translate(cameraTransform.fwd() * -moveSpeed);
		if (keyState[SDL_SCANCODE_S]) cameraTransform.translate(cameraTransform.fwd() * moveSpeed);
		if (keyState[SDL_SCANCODE_A]) cameraTransform.translate(cameraTransform.left() * -moveSpeed);
		if (keyState[SDL_SCANCODE_D]) cameraTransform.translate(cameraTransform.left() * moveSpeed);
	}
	else {
		firstMouse = true;
	}
}

static void initScene() {
	auto& triangle = Scene::get().scene.emplaceChild();
	triangle.transform().pos() = vec3(0, 0, 0);
	triangle.color() = glm::u8vec3(255, 0, 0);
	triangle.setName("triangle");

	auto& child_textured_triangle = triangle.emplaceChild();
	child_textured_triangle.transform().pos() = vec3(2, 0, 0);
	child_textured_triangle.color() = glm::u8vec3(0, 255, 0);
	child_textured_triangle.setName("textured_triangle");

	auto& child_textured_quad = child_textured_triangle.emplaceChild();
	child_textured_quad.transform().pos() = vec3(2, 0, 0);
	child_textured_quad.color() = glm::u8vec3(0, 0, 255);
	child_textured_quad.setName("child_textured_quad");

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

	ilInit();
	iluInit();
	ilutInit();

	MyWindow window("Sofo Engine", WINDOW_SIZE.x, WINDOW_SIZE.y);
	MyGUI gui(window.windowPtr(), window.contextPtr());

	init_opengl();

	// Init camera
	Scene::get().camera.transform().pos() = vec3(0, 1, 4);
	Scene::get().camera.transform().rotate(glm::radians(180.0), vec3(0, 1, 0));
	Scene::get().scene.setName("Scene");

	initScene();

	bool running = true;
	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	SDL_Event e;

	while (running) {
		const auto t0 = hrclock::now();
		display_func();
		reshape_func(WINDOW_SIZE.x, WINDOW_SIZE.y);
		gui.Begin();
		gui.render();
		gui.End();
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
			case SDL_DROPFILE:
				
				std::filesystem::path file = e.drop.file;

				if (file.extension() == ".obj" || file.extension() == ".OBJ" || file.extension() == ".fbx" || file.extension() == ".FBX") {
					auto& child = Scene::get().scene.emplaceChild();
					child.setName(generateRandomNameFromBase(file.filename().string()));
					for (auto& mesh : loadMeshesFromFile(file.string()))
					{
						auto& part = child.emplaceChild();
						part.setMesh(mesh);
						//part.setTextureImage(loadImageFromFile("Assets/Baker_house.png"));
						part.setName(mesh->name());
						part.setMatrix(mesh->modelMatrix());
					}
				}
				else if (file.extension() == ".png") {
					std::shared_ptr<Image> img = loadImageFromFile(file.string());

					if(Scene::get().selectedGO != nullptr)
						Scene::get().selectedGO->setTextureImage(img);

					std::cout << "Texture Filepath: " << file.string() << std::endl;
				}
				break;
			}
		}

		camera_func(Scene::get().camera.transform(), keyState, mouseX, mouseY, mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT));
	}

	return 0;
}