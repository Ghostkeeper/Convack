#Library to pack convex polygons into arbitrary shapes.
#Any copyright is dedicated to the public domain. See LICENSE.md for more details.

cmake_minimum_required(VERSION 3.16.3) #Oldest version it was tested with.

#Release configuration.
set(CONVACK_VERSION_MAJOR 0 CACHE STRING "Major release version. This must be incremented if consumers of older versions may need to change their code.")
set(CONVACK_VERSION_MINOR 0 CACHE STRING "Minor release version. This must be incremented if consumers of older versions don't need to change their code, but consumers of newer versions may need to change their code to downgrade.")
set(CONVACK_VERSION_PATCH 0 CACHE STRING "Patch release version. This must be incremented for every release where there are no changes to the API.")

project(convack VERSION ${CONVACK_VERSION_MAJOR}.${CONVACK_VERSION_MINOR}.${CONVACK_VERSION_PATCH} DESCRIPTION "Library to pack convex polygons into arbitrary shapes.")
