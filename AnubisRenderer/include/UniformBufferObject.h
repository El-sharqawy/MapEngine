#pragma once

#include <glad/glad.h>
#include <string>
#include "AnubisEnums.h"

class CUniformBufferObject
{
public:
	CUniformBufferObject();
	~CUniformBufferObject();
	void Initialize(GLsizeiptr bufferSize, EUniformBufferBindingPoints bindingPoint);
	void Destroy();

	void Bind() const;
	void Unbind() const;

	GLuint GetBufferID() const;
	GLsizeiptr GetBufferSize() const;
	GLuint GetBindingPointInt() const;
	EUniformBufferBindingPoints GetBindingPoint() const;
	void SetName(const std::string& name);

	void Update(const void* pData, GLsizeiptr size, GLintptr offset, bool bReallocation = true);


private:
	static GLint ms_iMaxUBOSize;

	std::string m_stName;
	GLuint m_uiBufferID;
	EUniformBufferBindingPoints m_eBindingPoint;
	GLsizeiptr m_uiBufferSize;
	bool m_bIsInitialized = false;
};