#ifndef AREASHADER_H
#define AREASHADER_H

#include "shader.h"



class AreaShader : public Shader
{
public:
	AreaShader();
	AreaShader(Vector3D bgColor_);

	Vector3D computeColor(const Ray& r,
		const std::vector<Shape*>& objList,
		const std::vector<LightSource*>& lsList) const;

};

#endif // AREASHADER_H