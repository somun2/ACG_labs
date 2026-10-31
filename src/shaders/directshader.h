#ifndef DIRECTSHADER_H
#define DIRECTSHADER_H

#include "shader.h"



class DirectShader : public Shader
{
public:
	DirectShader();
	DirectShader(Vector3D bgColor_);

	Vector3D computeColor(const Ray& r,
		const std::vector<Shape*>& objList,
		const std::vector<LightSource*>& lsList) const;

};

#endif // DIRECTSHADER_H