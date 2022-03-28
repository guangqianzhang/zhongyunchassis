cmake_minimum_required(VERSION 3.0.2)
project(chassis2)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11)
SET(CMAKE_BUILD_TYPE Debug)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  message_generation
)
add_message_files(
  FILES
  cmd.msg
  vcu_fbk.msg
  car_state.msg
)
generate_messages(
  DEPENDENCIES
  std_msgs
)
file(GLOB_RECURSE ALL_SRCS "*.cpp")
file(GLOB_RECURSE NODE_SRCS "src/node/*_node.cpp")
file(GLOB_RECURSE THIRD_PARTY_SRCS "third_party/*.cpp")
list(REMOVE_ITEM ALL_SRCS [${NODE_SRCS} ${THIRD_PARTY_SRCS}])

file(GLOB_RECURSE ALL_HS "*.h")
catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES chassis
 CATKIN_DEPENDS  roscpp rospy std_msgs message_runtime 
#  DEPENDS system_lib
)
include_directories(
include
  ${catkin_INCLUDE_DIRS}
  # /usr/include
)
add_library(socket
  # src/vcu_can/hh.cpp
  ${ALL_SRCS}
  ${ALL_HS}

)
add_dependencies(socket ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(socket
  ${catkin_LIBRARIES}
)
add_executable(vcucan src/vcu_node.cpp)
add_dependencies(vcucan ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(vcucan
  ${catkin_LIBRARIES}
  socket
)