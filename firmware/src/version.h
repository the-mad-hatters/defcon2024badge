#ifndef VERSION_H
#define VERSION_H

#define VERSION_MAJOR 1
#define VERSION_MINOR 3
#define VERSION_PATCH 0

// Stringify the version parts
#define STRINGIFY(x) #x
#define TOSTRING(x)  STRINGIFY(x)

// Create the version string
#define VERSION_STRING TOSTRING(VERSION_MAJOR) "." TOSTRING(VERSION_MINOR) "." TOSTRING(VERSION_PATCH)

#endif // VERSION_H
