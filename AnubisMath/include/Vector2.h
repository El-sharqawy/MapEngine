#pragma once

#include <glad/glad.h>
#include <glm/detail/type_vec2.hpp>

constexpr GLfloat VECTOR2_EPS = (1e-6f);	// geometry / vectors

/**
 * SVector2Df: A 2D vector struct.
 *
 * This struct represents a 2D vector with components. It provides
 * basic vector operations such as addition, subtraction, scalar multiplication,
 * dot product, length calculation, and normalization.
 *
 * Key features:
 * - Efficient type-based operations
 * - Accurate length and normalization calculations
 * - Support for dot product
 * - Conversion to and from GLM vectors for interoperability
 * - Clear and concise implementation adhering to Betty coding standards
 */
struct SVector2Df
{
	union
	{
		GLfloat x;
		GLfloat u;
		GLfloat r;
		GLfloat s;
		GLfloat width;
	};

	union
	{
		GLfloat y;
		GLfloat v;
		GLfloat g;
		GLfloat t;
		GLfloat height;
	};

	/**
	 * Default constructor, initializes all components to zero.
	 */
	SVector2Df() = default;

	/**
	 * Constructs an SVector2Df object with both x and y components initialized to the same value.
	 *
	 * @param val The initial value for both x and y components.
	 */
	SVector2Df(GLfloat val);

	/**
	 * Constructs an SVector2Df object with specified x and y components.
	 *
	 * @param _x The initial value for the x component.
	 * @param _y The initial value for the y component.
	 */
	SVector2Df(GLfloat _x, GLfloat _y);

	/**
	 * Constructs an SVector2Df object with both x and y components initialized to the same value.
	 *
	 * @param iVal The initial value for both x and y components.
	 */
	SVector2Df(GLint iVal);

	/**
	 * Constructs an SVector2Df object with specified x and y components.
	 *
	 * @param _x The initial value for the x component.
	 * @param _y The initial value for the y component.
	 */
	SVector2Df(GLint _x, GLint _y);

	/**
	 * Constructs an SVector2Df object by copying the components from a glm::vec2 vector.
	 *
	 * @param vec The glm::vec2 vector to copy from.
	 */
	SVector2Df(const glm::vec2& vec);

	/**
	 * Adds two SVector2Df objects component-wise.
	 *
	 * @param vec The SVector2Df object to add.
	 * @return The resulting SVector2Df object.
	 */
	SVector2Df operator+(const SVector2Df& vec);

	/**
	 * Subtracts one SVector2Df object from another component-wise.
	 *
	 * @param vec The SVector2Df object to subtract.
	 * @return The resulting SVector2Df object.
	 */
	SVector2Df operator-(const SVector2Df& vec);

	/**
	 * Multiplies two SVector2Df objects component-wise.
	 *
	 * @param vec The SVector2Df object to multiply.
	 * @return The resulting SVector2Df object.
	 */
	SVector2Df operator*(const SVector2Df& vec);

	/**
	 * Divides one SVector2Df object by another component-wise.
	 *
	 * @param vec The SVector2Df object to divide by.
	 * @return The resulting SVector2Df object.
	 */
	SVector2Df operator/(const SVector2Df& vec);

	/**
	 * Adds a scalar value to both components of an SVector2Df object.
	 *
	 * @param Val The scalar value to add.
	 * @return The resulting SVector2Df object.
	 */
	SVector2Df operator+(const GLfloat Val);

	/**
	 * Subtracts a scalar value from both components of an SVector2Df object.
	 *
	 * @param Val The scalar value to subtract.
	 * @return The resulting SVector2Df object.
	 */
	SVector2Df operator-(const GLfloat Val);

	/**
	 * Multiplies both components of an SVector2Df object by a scalar value.
	 *
	 * @param Val The scalar value to multiply by.
	 * @return The resulting SVector2Df object.
	 */
	SVector2Df operator*(const GLfloat Val);

	/**
	 * Divides both components of an SVector2Df object by a scalar value.
	 *
	 * @param Val The scalar value to divide by.
	 * @return The resulting SVector2Df object.
	 */
	SVector2Df operator/(const GLfloat Val);

	/**
	 * Adds another SVector2Df object to this one, modifying this object in-place.
	 *
	 * @param vec The SVector2Df object to add.
	 * @return A reference to this modified SVector2Df object.
	 */
	SVector2Df& operator+=(const SVector2Df& vec);

	/**
	 * Subtracts another SVector2Df object from this one, modifying this object in-place.
	 *
	 * @param vec The SVector2Df object to subtract.
	 * @return A reference to this modified SVector2Df object.
	 */
	SVector2Df& operator-=(const SVector2Df& vec);

	/**
	 * Multiplies this SVector2Df object by another one, modifying this object in-place.
	 *
	 * @param vec The SVector2Df object to multiply by.
	 * @return A reference to this modified SVector2Df object.
	 */
	SVector2Df& operator*=(const SVector2Df& vec);

	/**
	 * Divides this SVector2Df object by another one, modifying this object in-place.
	 *
	 * @param vec The SVector2Df object to divide by.
	 * @return A reference to this modified SVector2Df object.
	 */
	SVector2Df& operator/=(const SVector2Df& vec);

	/**
	 * Adds a scalar value to both components of this SVector2Df object, modifying it in-place.
	 *
	 * @param Val The scalar value to add.
	 * @return A reference to this modified SVector2Df object.
	 */
	SVector2Df& operator+=(const GLfloat Val)
	{
		x += Val;
		y += Val;
		return (*this);
	}

	/**
	 * Subtracts a scalar value from both components of this SVector2Df object, modifying it in-place.
	 *
	 * @param Val The scalar value to subtract.
	 * @return A reference to this modified SVector2Df object.
	 */
	SVector2Df& operator-=(const GLfloat Val)
	{
		x -= Val;
		y -= Val;
		return (*this);
	}

	/**
	 * Multiplies both components of this SVector2Df object by a scalar value, modifying it in-place.
	 *
	 * @param Val The scalar value to multiply by.
	 * @return A reference to this modified SVector2Df object.
	 */
	SVector2Df& operator*=(const GLfloat Val);

	/**
	 * Divides both components of this SVector2Df object by a scalar value, modifying it in-place.
	 *
	 * @param Val The scalar value to divide by.
	 * @return A reference to this modified SVector2Df object.
	 * @throws std::domain_error If the divisor is zero.
	 */
	SVector2Df& operator/=(const GLfloat Val);

	/**
	 * Compares two SVector2Df objects for equality.
	 *
	 * @param vec The SVector2Df object to compare to.
	 * @return true if the two objects are equal, false otherwise.
	 */
	bool operator == (const SVector2Df& vec) const;

	/**
	 * Compares two SVector2Df objects for inequality.
	 *
	 * @param vec The SVector2Df object to compare to.
	 * @return true if the two objects are not equal, false otherwise.
	 */
	bool operator != (const SVector2Df& vec) const;

	/**
	 * Compares SVector2Df and glm::vec2 objects for equality.
	 *
	 * @param vec The glm::vec2& vec object to compare to.
	 * @return true if the two objects are equal, false otherwise.
	 */
	bool operator == (const glm::vec2& vec) const;

	/**
	 * Compares SVector2Df and glm::vec2 objects for inequality.
	 *
	 * @param vec The glm::vec2& vec object to compare to.
	 * @return true if the two objects are not equal, false otherwise.
	 */
	bool operator != (const glm::vec2& vec) const;

	/**
	 * Allow to access the vector in array like style []
	 *
	 * @param index access param index
	 * @return actual index value
	 * @throws std::out_of_range If the index is out of 0-2 range.
	 */
	GLfloat& operator[](size_t index);

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
	const GLfloat operator[](size_t index) const;

	/**
	 * @brief Prefix increment operator (e.g., ++v).
	 *
	 * Increments both the x and y components of the vector before the result of
	 * the operation is used. This is generally more efficient than the postfix form.
	 *
	 * @return SVector2Df& A reference to the modified vector instance (*this).
	 */
	SVector2Df& operator++();

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
	SVector2Df operator++(GLint);

	/**
	 * @brief Prefix decrement operator (e.g., --v).
	 *
	 * Decrements both the x and y components of the vector before the result of
	 * the operation is used. This is generally more efficient than the postfix form.
	 *
	 * @return SVector2Df& A reference to the modified vector instance (*this).
	 */
	SVector2Df& operator--();

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
	SVector2Df operator--(GLint);

	/**
	 * @brief Unary negation operator (non-modifying).
	 *
	 * Calculates a new vector where every component is negated (multiplied by -1).
	 *
	 * @return SVector3Df A new vector instance with all components negated.
	 */
	SVector2Df operator-() const;

	/**
	 * @brief Unary plus operator (non-modifying).
	 *
	 * Returns a copy of the vector unchanged. This operator is primarily
	 * included for symmetry with the unary minus operator.
	 *
	 * @return SVector3Df A copy of the SVector3Df instance (*this).
	 */
	SVector2Df operator+() const;

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
	SVector2Df& operator=(const glm::vec2& vec2);

	/**
	 * @brief Implicit conversion operator to glm::vec2.
	 *
	 * Allows an SVector2Df instance to be implicitly converted to a glm::vec2 type,
	 * enabling direct assignment to GLM variables: glm::vec2 myGlmVec = myVec2;
	 * It is defined as 'const' because it does not modify the SVector2Df object.
	 *
	 * @return glm::vec2 A glm::vec2 instance with the same x and y values.
	 */
	operator glm::vec2() const;

	/**
	 * @brief Const GLfloat* conversion operator.
	 *
	 * Allows the SVector3Df object to be implicitly converted to a constant
	 * pointer to its underlying float array (starting at the 'x' component).
	 * This is useful for passing vector data directly to C-style functions,
	 * such as OpenGL API calls.
	 * This operator is marked 'const' as it provides read-only access.
	 *
	 * @return const GLfloat* A constant pointer to the first component (x).
	 */
	operator const GLfloat* () const;

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
	operator GLfloat* ();

	/**
	 * @brief Calculates the squared length of the vector.
	 *
	 * Computes the squared Euclidean length of the vector, defined as
	 * the sum of the squares of its components: x^2 + y^2.
	 * This operation is non-modifying and marked 'const'.
	 *
	 * @return GLfloat The squared length of the vector.
	 */
	GLfloat lengthSquared() const;

	/**
	 * Calculates the length (magnitude) of the SVector2Df object.
	 *
	 * @return The length of the vector.
	 */
	GLfloat length() const;

	/**
	 * Normalizes the SVector2Df object, making its length 1.
	 *
	 * @return A reference to this modified SVector2Df object.
	 * @throws std::domain_error If the vector is zero.
	 */
	SVector2Df& normalize();

	/**
	 * Converts the Vector2D object to a glm::vec2 object.
	 *
	 * @return The equivalent glm::vec2 object.
	 */
	glm::vec2 ToGLM() const;
};
