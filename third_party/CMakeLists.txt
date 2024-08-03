# codeshaunted - boyi_convert
# third_party/CMakeLists.txt
# third party CMake file
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

# bgfx.cmake
set(BGFX_BUILD_EXAMPLES OFF)
set(BGFX_BUILD_TOOLS OFF)
add_subdirectory("bgfx.cmake")

# zstd
set(ZSTD_BUILD_PROGRAMS OFF)
add_subdirectory("zstd/build/cmake")