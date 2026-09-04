#ifndef DEFINITION_OS_H
#define DEFINITION_OS_H

#if defined(_WIN32) || defined(_WIN64)
	#define OS_WINDOWS 1
#elif defined(__linux__)
	#define OS_LINUX 1
#elif defined(__APPLE__) && defined(__MACH__)
	#define OS_MAC 1
#endif

#endif /* DEFINITION_OS_H */