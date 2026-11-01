#ifndef PURESHADER_H
#define PURESHADER_H

#include "shader.h"



class PureShader : public Shader
{
public:
	PureShader();
	PureShader(Vector3D bgColor_);

	Vector3D computeColor(const Ray& r,
		const std::vector<Shape*>& objList,
		const std::vector<LightSource*>& lsList) const;

};

#endif // PURESHADER_H