#pragma once

#include "MemoryManager.h"

// Helper macro — wraps the boilerplate call arguments
#define MEM_FUNC   __FUNCTION__, __FILE__, __LINE__

// Engine System Tagged Allocation (Explicit Tags)
#define AnubisCNew(type, tag, align) (type*)CServiceLocator::Get<CMemoryManager>().TrackedMalloc(MEM_FUNC, #type, sizeof(type), tag, align)
#define AnubisCNewZero(type, count, tag, align) (type*)CServiceLocator::Get<CMemoryManager>().TrackedCalloc(MEM_FUNC, #type, count, sizeof(type), tag, align)
#define AnubisCNewZeroArr(type, count, tag, align) (type*)CServiceLocator::Get<CMemoryManager>().TrackedCalloc(MEM_FUNC, #type "[]", count, sizeof(type), tag, align)

// Arrays/Bytes
#define AnubisMalloc(size, tag, align) CServiceLocator::Get<CMemoryManager>().TrackedMalloc(MEM_FUNC, "raw_bytes", size, tag, align)
#define AnubisMallocArray(size, count, tag, align) CServiceLocator::Get<CMemoryManager>().TrackedMalloc(MEM_FUNC, "raw_bytes []", sizeof(type) * (count), tag, align)
#define AnubisCalloc(count, size, tag, align) CServiceLocator::Get<CMemoryManager>().TrackedCalloc(MEM_FUNC, "raw_bytes", count, size, tag, align)

// Reallocation (Pass NULL for typeName to keep the old one)
#define AnubisRealloc(ptr, size) CServiceLocator::Get<CMemoryManager>().TrackedRealloc(MEM_FUNC, ptr, size);

// Strings
#define AnubisStrdup(szSource, tag) CServiceLocator::Get<CMemoryManager>().TrackedStrdup(MEM_FUNC, szSource, "strdup(" #szSource ")", tag)

// Cleanup
#define AnubisFree(ptr) CServiceLocator::Get<CMemoryManager>().TrackedFree(MEM_FUNC, (void*)(ptr)); (ptr) = nullptr


#define AnubisNew(type, tag, align, ...) CServiceLocator::Get<CMemoryManager>().TrackedNew<type>(MEM_FUNC, tag, align, ##__VA_ARGS__)

#define AnubisDelete(ptr) CServiceLocator::Get<CMemoryManager>().TrackedDelete(MEM_FUNC, ptr); (ptr) = nullptr

#define AnubisNewArray(type, count, tag, align) CServiceLocator::Get<CMemoryManager>().TrackedNewArray<type>(MEM_FUNC, count, tag, align)

#define AnubisDeleteArray(ptr, type, count) CServiceLocator::Get<CMemoryManager>().TrackedDeleteArray<type>(MEM_FUNC, ptr, count); (ptr) = nullptr

// Default asset paths
#define LOG_PATH "Assets/Logs/EngineLog.txt"
#define ERROR_LOG_PATH "Assets/Logs/EngineErrorLog.txt"
#define CONFIG_PATH "Assets/Config/EngineConfig.json"
#define SHADERS_PATH "Assets/Config/EngineShaders.json"
#define TEXTURES_PATH "Assets/Config/EngineTextures.json"
