#include "neeshader.h"
#include "../materials/mirror.h"
#include "../materials/transmissive.h"

#include "../core/utils.h"
#include "../core/hemisphericalsampler.h"
#include "../materials/phong.h"
#include <iostream>
#include <cmath>





NEEShader::NEEShader(Vector3D bgColor_) :
	Shader(bgColor_)
{
}

Vector3D NEEShader::computeColor(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const
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
        if (r.countEmission) {
            return its.shape->getMaterial().getEmissiveRadiance();
        }
        return Vector3D(0.0);
    }

    Vector3D Le = its.shape->getMaterial().getEmissiveRadiance();
	Vector3D Lr = reflectedRadiance(its, wo, r.depth, objList, lsList);
    return Le + Lr;
    
}




Vector3D NEEShader::reflectedRadiance(Intersection& its, const Vector3D& wo, int depth, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const {
	Vector3D Ldir = directRadiance(its, wo, objList, lsList);
	Vector3D Lind = indirectRadiance(its, wo, depth, objList, lsList);
	return Ldir + Lind;
}

Vector3D NEEShader::directRadiance(Intersection& its, const Vector3D& wo, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const {
	
    Vector3D n = its.normal;
    Vector3D Ldir = (0.0);

    for (size_t lsIndex = 0; lsIndex < lsList.size(); lsIndex++) {
        Vector3D randLigPos = lsList.at(lsIndex)->sampleLightPosition();
        Vector3D wi = randLigPos - its.itsPoint;
        wi = wi.normalized();
        Ray r(its.itsPoint, wi);
        Intersection itst;
        Vector3D Li = Vector3D(0.0, 0.0, 0.0);

        if (Utils::getClosestIntersection(r, objList, itst) && itst.shape->getMaterial().isEmissive()) {
            Li = itst.shape->getMaterial().getEmissiveRadiance();
        }
        else {
            Li = Vector3D(0.0, 0.0, 0.0);
        }

        double Gxy = (max(0.0, dot(wi, n)) * max(0.0, dot(-wi, lsList.at(lsIndex)->getNormal()))) / ((randLigPos - its.itsPoint).length() * (randLigPos - its.itsPoint).length());
        double wi_n = dot(wi, n);
        Vector3D brdf = its.shape->getMaterial().getReflectance(n, wo, wi);
        Ldir += Li * brdf * Gxy * lsList.at(lsIndex)->getArea();
    }
	return Ldir;
}

Vector3D NEEShader::indirectRadiance(Intersection& its, const Vector3D& wo, int depth, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const {
	HemisphericalSampler sampler;
    Vector3D wi = sampler.getSample(its.normal);
	depth = depth + 1;
	Ray newR = Ray(its.itsPoint, wi, depth);
    newR.countEmission = false;
    Vector3D Lind = (0.0);
	if (depth < 5) {
        Vector3D n = its.normal;
        Vector3D brdf = its.shape->getMaterial().getReflectance(n, wo, wi);
		Lind = computeColor(newR, objList, lsList) * brdf * dot(wi, n) * 2 * PI;
	}
	return Lind;
}