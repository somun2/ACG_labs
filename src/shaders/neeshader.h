#ifndef NEESHADER_H
#define NEESHADER_H

#include "shader.h"



class NEEShader : public Shader
{
public:
	NEEShader();
	NEEShader(Vector3D bgColor_);

	Vector3D computeColor(const Ray& r,
		const std::vector<Shape*>& objList,
		const std::vector<LightSource*>& lsList) const;

	Vector3D reflectedRadiance(Intersection& its, const Vector3D& wo, int depth, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const;
	Vector3D directRadiance(Intersection& its, const Vector3D& wo, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const;
	Vector3D indirectRadiance(Intersection& its, const Vector3D& wo, int depth, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const;
};

#endif // NEESHADER_H