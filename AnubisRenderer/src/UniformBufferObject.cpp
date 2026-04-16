#include "UniformBufferObject.h"
#include "GLUtils.h"
#include "LogManager.h"

GLint CUniformBufferObject::ms_iMaxUBOSize = 0;

CUniformBufferObject::CUniformBufferObject()
	: m_uiBufferID(0), m_eBindingPoint(EUniformBufferBindingPoints::UBO_BP_CAMERA), m_uiBufferSize(0), m_stName("UniformBufferObject"), m_bIsInitialized(false)
{

}

CUniformBufferObject::~CUniformBufferObject()
{
	Destroy();
}

void CUniformBufferObject::Initialize(GLsizeiptr bufferSize, EUniformBufferBindingPoints bindingPoint)
{
	if (ms_iMaxUBOSize == 0)
	{
		glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &ms_iMaxUBOSize);
	}

	if (bufferSize > (GLsizeiptr)ms_iMaxUBOSize)
	{
		syserr("UBO {}: Size {} exceeds hardware limit of {}", m_stName, bufferSize, ms_iMaxUBOSize);
		return;
	}

	m_uiBufferID = Anubis::GL::CreateBuffer();

	if (!m_uiBufferID)
	{
		syserr("Failed to create Uniform Buffer Object");
		return;
	}

	if (Anubis::GL::IsGLVersionHigher(4, 5))
	{
		glNamedBufferStorage(m_uiBufferID, bufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
	}
	else
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_uiBufferID);
		glBufferData(GL_UNIFORM_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW);
	}

	m_bIsInitialized = true;
	m_eBindingPoint = bindingPoint;
	m_uiBufferSize = bufferSize;
	Bind();
}

void CUniformBufferObject::Destroy()
{
	Anubis::GL::DeleteBuffer(m_uiBufferID);
}

void CUniformBufferObject::Bind() const
{
	if (!m_bIsInitialized)
	{
		syserr("Attempted to bind uninitialized UBO: {}", m_stName);
		return;
	}

	glBindBufferBase(GL_UNIFORM_BUFFER, GetBindingPointInt(), m_uiBufferID);
}

void CUniformBufferObject::Unbind() const
{
	glBindBufferBase(GL_UNIFORM_BUFFER, GetBindingPointInt(), 0);
}

GLuint CUniformBufferObject::GetBufferID() const
{
	return (m_uiBufferID);
}

GLsizeiptr CUniformBufferObject::GetBufferSize() const
{
	return (m_uiBufferSize);
}

GLuint CUniformBufferObject::GetBindingPointInt() const
{
	return static_cast<GLuint>(m_eBindingPoint);
}

EUniformBufferBindingPoints CUniformBufferObject::GetBindingPoint() const
{
	return (m_eBindingPoint);
}

void CUniformBufferObject::SetName(const std::string& name)
{
	m_stName = name;
}

void CUniformBufferObject::Update(const void* pData, GLsizeiptr size, GLintptr offset, bool bReallocation)
{
	if (!m_bIsInitialized)
	{
		syserr("Attempted to update uninitialized UBO: {}", m_stName);
		return;
	}

	// 1. Boundary Check: Ensure the write fits
	if (size <= 0)
	{
		return;
	}

	if (size > (GLsizeiptr)ms_iMaxUBOSize)
	{
		syserr("UBO {}: Size {} exceeds hardware limit of {}", m_stName, size, ms_iMaxUBOSize);
		return;
	}

	if ((size + offset) > m_uiBufferSize)
	{
		if (!bReallocation)
		{
			syserr("Attempted to update UBO: {} with size larger than buffer capacity", m_stName);
			return;
		}

		m_uiBufferID = Anubis::GL::CreateBuffer(); // Attemp to Recreate buffer

		GLsizeiptr newSize = size + offset;

		if (newSize > (GLsizeiptr)ms_iMaxUBOSize)
		{
			syserr("UBO {}: Reallocation size {} exceeds limit!", m_stName, newSize);
			return;
		}

		if (Anubis::GL::IsGLVersionHigher(4, 5))
		{
			glNamedBufferStorage(m_uiBufferID, newSize, pData, GL_DYNAMIC_STORAGE_BIT);
		}
		else
		{
			glBindBuffer(GL_UNIFORM_BUFFER, m_uiBufferID);
			glBufferData(GL_UNIFORM_BUFFER, newSize, pData, GL_DYNAMIC_DRAW);
		}

		m_uiBufferSize = newSize;
		Bind();
		return; // Early exit after reallocation
	}

	// 2. Data Update
	if (Anubis::GL::IsGLVersionHigher(4, 5))
	{
		glNamedBufferSubData(m_uiBufferID, offset, size, pData);
	}
	else
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_uiBufferID);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, pData);
	}
}
