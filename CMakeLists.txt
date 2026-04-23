################################################################################
# Set minimum required version of cmake, project name and compile options
################################################################################
cmake_minimum_required(VERSION 3.5)
project(ld08_driver)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if(MSVC)
  add_compile_definitions(_USE_MATH_DEFINES)
endif()

################################################################################
# Find ament packages and libraries for ament and system dependencies
################################################################################
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(Boost REQUIRED system)

################################################################################
# Build
################################################################################
include_directories(include)

# Collect all sources except main.cpp for the shared component library
file(GLOB COMPONENT_SRC src/*.cpp)
list(REMOVE_ITEM COMPONENT_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# ── Composable component (shared library) ────────────────────────────────────
add_library(ld08_driver_component SHARED ${COMPONENT_SRC})
target_link_libraries(ld08_driver_component
  ${sensor_msgs_TARGETS}
  Boost::system
  pthread
  rclcpp::rclcpp
  rclcpp_components::component
  udev
)

# Register the component so it can be loaded by a container at runtime
rclcpp_components_register_node(
  ld08_driver_component
  PLUGIN "ld08_driver::LD08DriverComponent"
  EXECUTABLE ld08_driver_component_node
)

# ── Classic standalone executable ────────────────────────────────────────────
add_executable(ld08_driver src/main.cpp)
target_link_libraries(ld08_driver
  ld08_driver_component
  rclcpp::rclcpp
)

################################################################################
# Install
################################################################################
install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)

install(
  TARGETS ld08_driver_component
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(
  TARGETS ld08_driver
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

################################################################################
# Macro for ament package
################################################################################
ament_export_dependencies(rclcpp rclcpp_components sensor_msgs)
ament_export_include_directories(include)
ament_export_libraries(ld08_driver_component)
ament_package()