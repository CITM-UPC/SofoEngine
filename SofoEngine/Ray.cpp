#include "Ray.h"
#include "BoundingBox.h"
#include <iostream>

Ray::Ray() : Origin(0.0f), Direction(0.0f) {}
Ray::Ray(const glm::vec3& origin, const glm::vec3& direction) : Origin(origin), Direction(direction) {}

bool Ray::Intersects(const BoundingBox& bb, float& t) const {
    t = 0.0f;

    // Verificar si el origen del rayo está dentro de la BoundingBox
    if (bb.min.x <= Origin.x && Origin.x <= bb.max.x &&
        bb.min.y <= Origin.y && Origin.y <= bb.max.y &&
        bb.min.z <= Origin.z && Origin.z <= bb.max.z)
    {
        return true;
    }

    // Cálculo de las distancias de intersección en cada eje
    float invDx = (Direction.x != 0.0f) ? 1.0f / Direction.x : std::numeric_limits<float>::infinity();
    float invDy = (Direction.y != 0.0f) ? 1.0f / Direction.y : std::numeric_limits<float>::infinity();
    float invDz = (Direction.z != 0.0f) ? 1.0f / Direction.z : std::numeric_limits<float>::infinity();

    // Intervalos en X
    float tmin = (bb.min.x - Origin.x) * invDx;
    float tmax = (bb.max.x - Origin.x) * invDx;
    if (tmin > tmax) std::swap(tmin, tmax);

    // Intervalos en Y
    float tymin = (bb.min.y - Origin.y) * invDy;
    float tymax = (bb.max.y - Origin.y) * invDy;
    if (tymin > tymax) std::swap(tymin, tymax);

    // Comprobar si los intervalos de X e Y se solapan
    if ((tmin > tymax) || (tymin > tmax)) return false;

    // Ajustar tmin y tmax al solapamiento de los intervalos
    if (tymin > tmin) tmin = tymin;
    if (tymax < tmax) tmax = tymax;

    // Intervalos en Z
    float tzmin = (bb.min.z - Origin.z) * invDz;
    float tzmax = (bb.max.z - Origin.z) * invDz;
    if (tzmin > tzmax) std::swap(tzmin, tzmax);

    // Comprobar si los intervalos solapan también en Z
    if ((tmin > tzmax) || (tzmin > tmax)) return false;

    // Ajustar tmin y tmax al solapamiento final de los intervalos
    if (tzmin > tmin) tmin = tzmin;
    if (tzmax < tmax) tmax = tzmax;

    // tmin contiene la distancia de la primera intersección válida
    t = tmin;

    // Devolver verdadero si hay intersección
    return true;
}


bool Ray::Intersects(const BoundingBox& bb) const
{
    float distance;
    return Intersects(bb, distance);
}

bool Ray::IntersectsTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, float& t) const
{
    const glm::vec3 E1 = b - a;
    const glm::vec3 E2 = c - a;
    const glm::vec3 N = cross(E1, E2);
    const float det = -glm::dot(Direction, N);
    const float invdet = 1.f / det;
    const glm::vec3 AO = Origin - a;
    const glm::vec3 DAO = glm::cross(AO, Direction);
    const float u = glm::dot(E2, DAO) * invdet;
    const float v = -glm::dot(E1, DAO) * invdet;
    t = glm::dot(AO, N) * invdet;
    return (det >= 1e-6f && t >= 0.0f && u >= 0.0f && v >= 0.0f && (u + v) <= 1.0f);
}