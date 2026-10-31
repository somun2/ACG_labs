#include "arealightsource.h"
#include <random>

AreaLightSource::AreaLightSource(Square* areaLightsource_) :
    myAreaLightsource(areaLightsource_)
{ }



Vector3D AreaLightSource::getIntensity() const
{
    return myAreaLightsource->getMaterial().getEmissiveRadiance();
}


Vector3D AreaLightSource::sampleLightPosition()   const
{
    //FILL(...)
    // 
    //New Randam Pos inside Area Lightsource
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, 1.0);

    double u = dis(gen);
    double v = dis(gen);


    return myAreaLightsource->corner
        + myAreaLightsource->v1 * u
        + myAreaLightsource->v2 * v;
}

