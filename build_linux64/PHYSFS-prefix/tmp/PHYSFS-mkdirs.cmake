# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/runner/work/OpenJKDF2/OpenJKDF2/lib/physfs")
  file(MAKE_DIRECTORY "/home/runner/work/OpenJKDF2/OpenJKDF2/lib/physfs")
endif()
file(MAKE_DIRECTORY
  "/home/runner/work/OpenJKDF2/OpenJKDF2/build_linux64/physfs"
  "/home/runner/work/OpenJKDF2/OpenJKDF2/build_linux64/physfs"
  "/home/runner/work/OpenJKDF2/OpenJKDF2/build_linux64/PHYSFS-prefix/tmp"
  "/home/runner/work/OpenJKDF2/OpenJKDF2/build_linux64/PHYSFS-prefix/src/PHYSFS-stamp"
  "/home/runner/work/OpenJKDF2/OpenJKDF2/build_linux64/PHYSFS-prefix/src"
  "/home/runner/work/OpenJKDF2/OpenJKDF2/build_linux64/PHYSFS-prefix/src/PHYSFS-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/runner/work/OpenJKDF2/OpenJKDF2/build_linux64/PHYSFS-prefix/src/PHYSFS-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/runner/work/OpenJKDF2/OpenJKDF2/build_linux64/PHYSFS-prefix/src/PHYSFS-stamp${cfgdir}") # cfgdir has leading slash
endif()
