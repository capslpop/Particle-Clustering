# CMakeList.txt : CMake project for TestFluid, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (TestFluid "TestFluid.cpp" "main.h" "Graphics/Camera.cpp" "Graphics/Camera.h" "Graphics/pallet.h" "Graphics/RenderPass.cpp" "Graphics/RenderPass.h"  "Graphics/Window.cpp" "Graphics/Window.h" "Graphics/FileHelper.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TestFluid PROPERTY CXX_STANDARD 20)
endif()

find_package(imgui REQUIRED)
target_link_libraries(TestFluid imgui::imgui)

# TODO: Add tests and install targets if needed.
find_package (glfw3 REQUIRED)
target_link_libraries (TestFluid glfw)

find_package (glm REQUIRED)
target_link_libraries (TestFluid glm::glm)

# include GLAD as a way to link with openGL
find_package(glad REQUIRED)
target_link_libraries(TestFluid glad::glad)

add_custom_target(copy_shaders
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_LIST_DIR}/copy_shaders.cmake
)
add_dependencies(TestFluid copy_shaders)