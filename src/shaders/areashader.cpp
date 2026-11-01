#include "areashader.h"
#include "../materials/mirror.h"
#include "../materials/transmissive.h"

#include "../core/utils.h"
#include "../core/hemisphericalsampler.h"
#include "../materials/phong.h"
#include <iostream>
#include <cmath>



AreaShader::AreaShader(Vector3D bgColor_) :
	Shader(bgColor_)
{
}

Vector3D AreaShader::computeColor(const Ray& r, const std::vector<Shape*>& objList,
    const std::vector<LightSource*>& lsList) const
{
    Intersection its;
    if (!Utils::getClosestIntersection(r, objList, its)) {
        return bgColor;
    }

    const Vector3D n = its.normal;
    const Vector3D wo = (-r.d);


    if (its.shape->getMaterial().hasTransmission()) {

        if (r.depth >= 5) {
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
        return Vector3D(1.0, 1.0, 1.0);
    }

    Vector3D Lo = Vector3D(0.0, 0.0, 0.0);

    HemisphericalSampler sampler;

    for (size_t lsIndex = 0; lsIndex < lsList.size(); lsIndex++) {
        for (int i = 0; i < 256; i++) {
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

            double Gxy = (max(0.0,dot(wi, n)) * max(0.0,dot(-wi, lsList.at(lsIndex)->getNormal())))/((randLigPos - its.itsPoint).length() * (randLigPos - its.itsPoint).length());
            double wi_n = dot(wi, n);
            Vector3D brdf = its.shape->getMaterial().getReflectance(n, wo, wi);
            Lo += Li * brdf * wi_n * Gxy;
        }
    }
    double at = 0.2;
    return (Lo/256) * lsList.at(0)->getArea() + at * its.shape->getMaterial().getDiffuseReflectance();
}
