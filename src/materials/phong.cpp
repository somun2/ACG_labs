#include "phong.h"
#include "../core/utils.h"
#include <iostream>

Phong::Phong()
{ }

Phong::Phong(Vector3D Kd_, Vector3D Ks_, float alpha_):
rho_d(Kd_), Ks(Ks_), alpha(alpha_){}


Vector3D Phong::getReflectance(const Vector3D& n, const Vector3D& wo,
    const Vector3D& wi) const {


    double pi = PI;
    Vector3D wr = n*(2.0*dot(n, wi)) - wi;
    double x = dot(wo, wr);

    if (x < 0.0) x = 0.0;

    Vector3D r = rho_d / pi;
    if (x > 0.0) {
        r += ((alpha +2)/(2*pi))*Ks * pow(x, alpha);
    }

    return r;
}

double Phong::getIndexOfRefraction() const
{
    std::cout << "Warning! Calling \"Material::getIndexOfRefraction()\" for a non-transmissive material"
              << std::endl;

    return -1;
}


Vector3D Phong::getEmissiveRadiance() const
{
    return Vector3D(0.0);
}


Vector3D Phong::getDiffuseReflectance() const
{
    return rho_d;
}

