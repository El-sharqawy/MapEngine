
#include "Stdafx.h"
#include "Vector2.h"

/**
 * Constructs an SVector2Df object with both x and y components initialized to the same value.
 *
 * @param val The initial value for both x and y components.
 */
SVector2Df::SVector2Df(GLfloat val)
{
	x = y = val;
}

/**
 * Constructs an SVector2Df object with both x and y components initialized to the same value.
 *
 * @param val The initial value for both x and y components.
 */
SVector2Df::SVector2Df(GLfloat _x, GLfloat _y)
{
	x = _x;
	y = _y;
}

/**
 * Constructs an SVector2Df object with both x and y components initialized to the same value.
 *
 * @param val The initial value for both x and y components.
 */
SVector2Df::SVector2Df(GLint iVal)
{
	x = y = static_cast<GLfloat>(iVal);
}

/**
 * Constructs an SVector2Df object with specified x and y components.
 *
 * @param _x The initial value for the x component.
 * @param _y The initial value for the y component.
 */
SVector2Df::SVector2Df(GLint _x, GLint _y)
{
	x = static_cast<GLfloat>(_x);
	y = static_cast<GLfloat>(_y);
}

/**
 * Constructs an SVector2Df object by copying the components from a glm::vec2 vector.
 *
 * @param vec The glm::vec2 vector to copy from.
 */
SVector2Df::SVector2Df(const glm::vec2& vec)
{
	x = vec.x;
	y = vec.y;
}

/**
 * Adds two SVector2Df objects component-wise.
 *
 * @param vec The SVector2Df object to add.
 * @return The resulting SVector2Df object.
 */
SVector2Df SVector2Df::operator+(const SVector2Df& vec)
{
	return (SVector2Df(x + vec.x, y + vec.y));
}

/**
 * Adds two SVector2Df objects component-wise.
 *
 * @param vec The SVector2Df object to add.
 * @return The resulting SVector2Df object.
 */
SVector2Df SVector2Df::operator-(const SVector2Df& vec)
{
	return (SVector2Df(x - vec.x, y - vec.y));
}

/**
 * Multiplies two SVector2Df objects component-wise.
 *
 * @param vec The SVector2Df object to multiply.
 * @return The resulting SVector2Df object.
 */
SVector2Df SVector2Df::operator*(const SVector2Df& vec)
{
	return (SVector2Df(x * vec.x, y * vec.y));
}

/**
 * Divides one SVector2Df object by another component-wise.
 *
 * @param vec The SVector2Df object to divide by.
 * @return The resulting SVector2Df object.
 */
SVector2Df SVector2Df::operator/(const SVector2Df& vec)
{
	GLfloat tX = x;
	GLfloat tY = y;
	if (vec.x != 0.0)
	{
		tX = x / vec.x;
	}
	if (vec.y != 0.0)
	{
		tY = y / vec.y;
	}

	return SVector2Df(tX, tY);
}

/**
 * Adds a scalar value to both components of an SVector2Df object.
 *
 * @param Val The scalar value to add.
 * @return The resulting SVector2Df object.
 */
SVector2Df SVector2Df::operator+(const GLfloat Val)
{
	return (SVector2Df(x + Val, y + Val));
}

/**
 * Subtracts a scalar value from both components of an SVector2Df object.
 *
 * @param Val The scalar value to subtract.
 * @return The resulting SVector2Df object.
 */
SVector2Df SVector2Df::operator-(const GLfloat Val)
{
	return (SVector2Df(x - Val, y - Val));
}

/**
 * Multiplies both components of an SVector2Df object by a scalar value.
 *
 * @param Val The scalar value to multiply by.
 * @return The resulting SVector2Df object.
 */
SVector2Df SVector2Df::operator*(const GLfloat Val)
{
	return (SVector2Df(x * Val, y * Val));
}

/**
 * Divides both components of an SVector2Df object by a scalar value.
 *
 * @param Val The scalar value to divide by.
 * @return The resulting SVector2Df object.
 */
SVector2Df SVector2Df::operator/(const GLfloat Val)
{
	GLfloat fX = x;
	GLfloat fY = y;
	if (Val != 0.0)
	{
		fX = x / Val;
		fY = y / Val;
	}

	return SVector2Df(fX, fY);
}

/**
 * Adds another SVector2Df object to this one, modifying this object in-place.
 *
 * @param vec The SVector2Df object to add.
 * @return A reference to this modified SVector2Df object.
 */
SVector2Df& SVector2Df::operator+=(const SVector2Df& vec)
{
	x += vec.x;
	y += vec.y;
	return (*this);
}

/**
 * Subtracts another SVector2Df object from this one, modifying this object in-place.
 *
 * @param vec The SVector2Df object to subtract.
 * @return A reference to this modified SVector2Df object.
 */
SVector2Df& SVector2Df::operator-=(const SVector2Df& vec)
{
	x -= vec.x;
	y -= vec.y;
	return (*this);
}

/**
 * Multiplies this SVector2Df object by another one, modifying this object in-place.
 *
 * @param vec The SVector2Df object to multiply by.
 * @return A reference to this modified SVector2Df object.
 */
SVector2Df& SVector2Df::operator*=(const SVector2Df& vec)
{
	x *= vec.x;
	y *= vec.y;
	return (*this);
}

/**
 * Divides this SVector2Df object by another one, modifying this object in-place.
 *
 * @param vec The SVector2Df object to divide by.
 * @return A reference to this modified SVector2Df object.
 */
SVector2Df& SVector2Df::operator/=(const SVector2Df& vec)
{
	if (vec.x != 0.0)
	{
		x /= vec.x;
	}
	if (vec.y != 0.0)
	{
		y /= vec.y;
	}
	return (*this);
}

/**
 * Multiplies both components of this SVector2Df object by a scalar value, modifying it in-place.
 *
 * @param Val The scalar value to multiply by.
 * @return A reference to this modified SVector2Df object.
 */
SVector2Df& SVector2Df::operator*=(const GLfloat Val)
{
	x *= Val;
	y *= Val;
	return (*this);
}

/**
 * Divides both components of this SVector2Df object by a scalar value, modifying it in-place.
 *
 * @param Val The scalar value to divide by.
 * @return A reference to this modified SVector2Df object.
 * @throws std::domain_error If the divisor is zero.
 */
SVector2Df& SVector2Df::operator/=(const GLfloat Val)
{
	if (Val != 0.0f)
	{
		x /= Val;
		y /= Val;
	}
	return (*this);
}

/**
 * Compares two SVector2Df objects for equality.
 *
 * @param vec The SVector2Df object to compare to.
 * @return true if the two objects are equal, false otherwise.
 */
bool SVector2Df::operator == (const SVector2Df& vec) const
{
	bool bEqualX = std::fabs(x - vec.x) <= VECTOR2_EPS;
	bool bEqualY = std::fabs(y - vec.y) <= VECTOR2_EPS;

	return (bEqualX && bEqualY);
}

/**
 * Compares two SVector2Df objects for inequality.
 *
 * @param vec The SVector2Df object to compare to.
 * @return true if the two objects are not equal, false otherwise.
 */
bool SVector2Df::operator != (const SVector2Df& vec) const
{
	return !(*this == vec);
}

/**
 * Compares SVector2Df and glm::vec2 objects for equality.
 *
 * @param vec The glm::vec2& vec object to compare to.
 * @return true if the two objects are equal, false otherwise.
 */
bool SVector2Df::operator == (const glm::vec2& vec) const
{
	bool bEqualX = std::fabs(x - vec.x) <= VECTOR2_EPS;
	bool bEqualY = std::fabs(y - vec.y) <= VECTOR2_EPS;

	return (bEqualX && bEqualY);
}

/**
 * Compares SVector2Df and glm::vec2 objects for inequality.
 *
 * @param vec The glm::vec2& vec object to compare to.
 * @return true if the two objects are not equal, false otherwise.
 */
bool SVector2Df::operator != (const glm::vec2& vec) const
{
	return !(*this == vec);
}

/**
 * Allow to access the vector in array like style []
 *
 * @param index access param index
 * @return actual index value
 * @throws std::out_of_range If the index is out of 0-2 range.
 */
GLfloat& SVector2Df::operator[](size_t index)
{
	switch (index)
	{
	case 0:
		return x;
	case 1:
		return y;
	default:
		return x;
	}
}

/**
 * @brief Subscript operator (const version).
 *
 * Provides read-only access to the vector components using an index (0=x, 1=y).
 * Returns the value of the component.
 * If the index is out of bounds, it defaults to returning the value of the x component (index 0).
 * This operator is marked 'const' as it does not modify the vector.
 *
 * @param index The zero-based index of the component to access (0o r 1).
 * @return const GLfloat The value of the specified component (x or y).
 */
const GLfloat SVector2Df::operator[](size_t index) const
{
	switch (index)
	{
	case 0:
		return x;
	case 1:
		return y;
	default:
		return x;
	}
}

/**
 * @brief Prefix increment operator (e.g., ++v).
 *
 * Increments both the x and y components of the vector before the result of
 * the operation is used. This is generally more efficient than the postfix form.
 *
 * @return SVector2Df& A reference to the modified vector instance (*this).
 */
SVector2Df& SVector2Df::operator++()
{
	++this->x;
	++this->y;
	return (*this);
}

/**
 * @brief Postfix increment operator (e.g., v++).
 *
 * Increments both the x and y components of the vector after the original
 * value of the vector is retrieved and used.
 * This function returns a copy of the vector's state *before* modification,
 * ensuring standard C++ behavior for expressions like v2 = v1++.
 *
 * @param GLint The dummy parameter used to distinguish this as the postfix operator.
 * @return SVector2Df A copy of the vector instance before the increment.
 */
SVector2Df SVector2Df::operator++(GLint)
{
	SVector2Df temp = *this; // 1. Save the original state (required for postfix).
	++this->x;              // 2. Increment the actual object.
	++this->y;
	return temp;            // 3. Return the saved original state.
}

/**
 * @brief Prefix decrement operator (e.g., --v).
 *
 * Decrements both the x and y components of the vector before the result of
 * the operation is used. This is generally more efficient than the postfix form.
 *
 * @return SVector2Df& A reference to the modified vector instance (*this).
 */
SVector2Df& SVector2Df::operator--()
{
	--this->x;
	--this->y;
	return (*this);
}

/**
 * @brief Postfix decrement operator (e.g., v--).
 *
 * Decrements both the x and y components of the vector after the original
 * value of the vector is retrieved and used.
 * This function returns a copy of the vector's state *before* modification,
 * ensuring standard C++ behavior for expressions like v2 = v1--.
 *
 * @param GLint The dummy parameter used to distinguish this as the postfix operator.
 * @return SVector2Df A copy of the vector instance before the decrement.
 */
SVector2Df SVector2Df::operator--(GLint)
{
	SVector2Df temp = *this; // 1. Save the original state (required for postfix).
	--this->x;              // 2. Decrement the actual object.
	--this->y;
	return temp;            // 3. Return the saved original state.
}

/**
 * @brief Unary negation operator (non-modifying).
 *
 * Calculates a new vector where every component is negated (multiplied by -1).
 *
 * @return SVector2Df A new vector instance with all components negated.
 */
SVector2Df SVector2Df::operator-() const
{
	return SVector2Df(-x, -y);
}

/**
 * @brief Unary plus operator (non-modifying).
 *
 * Returns a copy of the vector unchanged. This operator is primarily
 * included for symmetry with the unary minus operator.
 *
 * @return SVector2Df A copy of the SVector2Df instance (*this).
 */
SVector2Df SVector2Df::operator+() const
{
	return *this;
}

/**
 * @brief Assignment operator overload for glm::vec2.
 *
 * Allows direct assignment from a GLM vector to this SVector2Df instance,
 * facilitating smooth interoperability between the two math libraries.
 * e.g., SVector2Df myVec; myVec = glm::vec2(5.0f, 10.0f);
 *
 * @param vec2 The constant reference to the glm::vec2 source vector.
 * @return SVector2Df& A reference to the modified SVector2Df instance (*this).
 */
SVector2Df& SVector2Df::operator=(const glm::vec2& vec2)
{
	this->x = vec2.x;
	this->y = vec2.y;
	return (*this);
}

/**
 * @brief Implicit conversion operator to glm::vec2.
 *
 * Allows an SVector2Df instance to be implicitly converted to a glm::vec2 type,
 * enabling direct assignment to GLM variables: glm::vec2 myGlmVec = myVec2;
 * It is defined as 'const' because it does not modify the SVector2Df object.
 *
 * @return glm::vec2 A glm::vec2 instance with the same x and y values.
 */
SVector2Df::operator glm::vec2() const
{
	return glm::vec2(this->x, this->y);
}

/**
 * @brief Const GLfloat* conversion operator.
 *
 * Allows the SVector2Df object to be implicitly converted to a constant
 * pointer to its underlying float array (starting at the 'x' component).
 * This is useful for passing vector data directly to C-style functions,
 * such as OpenGL API calls.
 * This operator is marked 'const' as it provides read-only access.
 *
 * @return const GLfloat* A constant pointer to the first component (x).
 */
SVector2Df::operator const GLfloat* () const
{
	return (&(x));
}

/**
 * @brief GLfloat* conversion operator.
 *
 * Allows the SVector2Df object to be implicitly converted to a pointer
 * to its underlying float array (starting at the 'x' component).
 * This is useful for passing vector data directly to C-style functions,
 * such as OpenGL API calls.
 *
 * @return GLfloat* A pointer to the first component (x).
 */
SVector2Df::operator GLfloat* ()
{
	return (&(x));
}

/**
 * @brief Calculates the squared length of the vector.
 *
 * Computes the squared Euclidean length of the vector, defined as
 * the sum of the squares of its components: x^2 + y^2.
 * This operation is non-modifying and marked 'const'.
 *
 * @return GLfloat The squared length of the vector.
 */
GLfloat SVector2Df::lengthSquared() const
{
	return (x * x + y * y);
}

/**
 * Calculates the length (magnitude) of the SVector2Df object.
 *
 * @return The length of the vector.
 */
GLfloat SVector2Df::length() const
{
	return (std::sqrt(x * x + y * y)); // Corrected the calculation
}

/**
 * Normalizes the SVector2Df object, making its length 1.
 *
 * @return A reference to this modified SVector2Df object.
 * @throws std::domain_error If the vector is zero.
 */
SVector2Df& SVector2Df::normalize()
{
	GLfloat Len = length();
	if (Len != 0.0)
	{
		x /= Len;
		y /= Len;
	}
	return (*this);
}

/**
 * Converts the Vector2D object to a glm::vec2 object.
 *
 * @return The equivalent glm::vec2 object.
 */
glm::vec2 SVector2Df::ToGLM() const
{
	return (glm::vec2(x, y));
}
