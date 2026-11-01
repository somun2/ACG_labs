#include "pureshader.h"
#include "../materials/mirror.h"
#include "../materials/transmissive.h"

#include "../core/utils.h"
#include "../core/hemisphericalsampler.h"
#include "../materials/phong.h"
#include <iostream>
#include <cmath>




PureShader::PureShader(Vector3D bgColor_) :
	Shader(bgColor_)
{
}

Vector3D PureShader::computeColor(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const
{
    Intersection its;
    if (!Utils::getClosestIntersection(r, objList, its)) {
        return bgColor;
    }

	int maxDepth = 5;
    const Vector3D n = its.normal;
    const Vector3D wo = (-r.d);


    if (its.shape->getMaterial().hasTransmission()) {

        if (r.depth >= maxDepth) {
            return Vector3D(0.0, 0.0, 0.0);
        }

        Vector3D normal = n;

        // Normal oriented against the incoming ray
        if (dot(r.d, normal) > 0.0) {
            normal = -normal;
        }

        double mu_t = its.shape->getMaterial().getIndexOfRefraction();

        // Determine whether the ray is entering or leaving
        // the sphere, using the original geometric normal.
        if (dot(r.d, n) > 0.0) {
            mu_t = 1.0 / mu_t;
        }

        double n_wo = dot(normal, wo);
        double radicand = 1.0 - mu_t * mu_t * (1.0 - n_wo * n_wo);

        // Total internal reflection
        if (radicand < 0.0) {
            Vector3D wr = Utils::computeReflectionDirection(r.d.normalized(), normal);
            Ray reflectedRay(its.itsPoint, wr, r.depth + 1);
            return computeColor(reflectedRay, objList, lsList);
        }

        // Refraction
        Vector3D wt = -mu_t * wo + normal * (mu_t * n_wo - std::sqrt(radicand));
        wt = wt.normalized();
        Ray transmittedRay(its.itsPoint, wt, r.depth + 1);
        return computeColor(transmittedRay, objList, lsList);
    }





    if (its.shape->getMaterial().hasSpecular()) {
        if (r.depth >= 5) {
            return Vector3D(0.0, 0.0, 0.0);
        }

        Vector3D normal = n;

        if (dot(r.d, normal) > 0.0) {
            normal = -normal;
        }

        Vector3D wr = Utils::computeReflectionDirection(r.d.normalized(), normal).normalized();
        Vector3D reflectedOrigin = its.itsPoint;
        Ray reflectedRay(reflectedOrigin, wr, r.depth + 1);
        return computeColor(reflectedRay, objList, lsList);
    }

    if (its.shape->getMaterial().isEmissive()) {
        return  its.shape->getMaterial().getEmissiveRadiance();
    }

    Vector3D Lo = its.shape->getMaterial().getEmissiveRadiance();

    HemisphericalSampler sampler;
	int N = 256;

    
    Vector3D wi = sampler.getSample(its.normal);
    if (r.depth < maxDepth) {
        Vector3D brfd = its.shape->getMaterial().getReflectance(n, wo, wi);
        Ray newR = Ray(its.itsPoint, wi, r.depth + 1);
        Lo += computeColor(newR, objList, lsList) * brfd * (dot(its.normal, wi)) * 2 * PI;
    }
    //printf("Lo: %f, %f, %f\n", Lo.x, Lo.y, Lo.z);
    return Lo;
	
    
}
