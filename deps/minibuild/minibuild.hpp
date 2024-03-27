#ifndef _MINIBUILD_HPP_
#define _MINIBUILD_HPP_

#if defined(_WIN32)
#include <windows.h>
#undef max
#endif

namespace mini {
void minibuild_project_rebuild_implementation(int argc, char** argv, const char* file);
}

#ifdef MINIBUILD_IMPLEMENTATION
namespace mini {
void minibuild_project_rebuild_implementation(int argc, char** argv, const char* file) {
}
}
#endif // MINIBUILD_IMPLEMENTATION

#define MINI_REBUILD(argc, argv) minibuild_project_rebuild_implementation(argc, argv, __FILE__)

#endif // _MINIBUILD_HPP_
