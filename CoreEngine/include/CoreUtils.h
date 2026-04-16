#pragma once

#include <string>

namespace Anubis
{
	std::string GetFileNameModern(const char* full_path);

    /**
     * @brief Safely deletes a pointer and sets it to nullptr.
     *
     * This template function checks if the provided pointer is non-null,
     * deletes the object it points to, and then sets the pointer to nullptr
     * to prevent dangling references.
     *
     * @tparam T The type of the object being pointed to.
     * @param ptr Reference to the pointer to be deleted.
     */
    template<typename T>
    void SafeDelete(T*& ptr)
    {
        if (ptr)
        {
            delete ptr;
            ptr = nullptr;
        }
    }

}