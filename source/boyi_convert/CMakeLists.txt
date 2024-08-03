# codeshaunted - boyi_convert
# source/boyi_convert/CMakeLists.txt
# source file directory CMake file
# Copyright 2024 codeshaunted
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http:#www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set(BOYI_CONVERT_SOURCE_FILES
	"${CMAKE_CURRENT_SOURCE_DIR}/main.cc")

set(BOYI_CONVERT_INCLUDE_DIRECTORIES
	"${CMAKE_SOURCE_DIR}/third_party"
    "${CMAKE_SOURCE_DIR}/third_party/bgfx.cmake"
    "${CMAKE_SOURCE_DIR}/third_party/zstd/lib"
    "${CMAKE_SOURCE_DIR}/third_party/cxxopts/include"
	"${PROJECT_BINARY_DIR}/source/boyi_convert")

set(BOYI_CONVERT_LINK_LIBRARIES
	bx bimg bimg_decode bimg_encode libzstd)

set(BOYI_CONVERT_COMPILE_DEFINITIONS)

#configure_file("${CMAKE_SOURCE_DIR}/include/boyi_convert/config.hh.in" "config.hh")

add_executable(boyi_convert ${BOYI_CONVERT_SOURCE_FILES})

target_include_directories(boyi_convert PUBLIC ${BOYI_CONVERT_INCLUDE_DIRECTORIES})

target_link_libraries(boyi_convert PUBLIC ${BOYI_CONVERT_LINK_LIBRARIES})

target_compile_definitions(boyi_convert PUBLIC ${BOYI_CONVERT_COMPILE_DEFINITIONS})