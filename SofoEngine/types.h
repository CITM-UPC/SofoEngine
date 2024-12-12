#pragma once

#include <glm/glm.hpp>

using mat4 = glm::dmat4;
using mat4f = glm::mat4;
using vec4 = glm::dvec4;
using vec4f = glm::vec4;
using vec3 = glm::dvec3;
using vec3f = glm::vec3;
using vec2 = glm::dvec2;
using vec2f = glm::vec2;

using quat = glm::dquat;
using quatf = glm::quat;

namespace Colors {
	const glm::u8vec3 Red( 255, 0, 0);
	const glm::u8vec3 Green( 0, 255, 0);
	const glm::u8vec3 Blue( 0, 0, 255);
};
