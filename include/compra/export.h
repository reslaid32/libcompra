#ifndef LIBCOMPRA_EXPORT_H
#define LIBCOMPRA_EXPORT_H

#if defined(_WIN32) || defined(_WIN64)
#ifdef LIBCOMPRA_LIBRARY_BUILD
#define LIBCOMPRA_API __declspec(dllexport)
#else
#define LIBCOMPRA_API __declspec(dllimport)
#endif
#else
#ifdef LIBCOMPRA_LIBRARY_BUILD
#define LIBCOMPRA_API  __attribute__((visibility("default")))
#else
#define LIBCOMPRA_API
#endif
#endif

#endif // LIBCOMPRA_EXPORT_H