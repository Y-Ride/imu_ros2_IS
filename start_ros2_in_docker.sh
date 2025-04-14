#!/bin/bash

# Make log files if they don't exist
mkdir -p /logs

# Source ROS 2 setup
source /opt/ros/${ROS_DISTRO}/setup.bash
source /ros2_ws/install/setup.bash 

# Run ROS 2 launch commands inside the container
ros2 launch inertial_sense_ros2 inertial_sense_ros2.launch.py > /logs/inertial_sense_ros2_node.log 2>&1 &
# Integrated gps_time_publisher into the inertial sense sdk so the following is no longer needed
# ros2 run gps_time_publisher gps_time_publisher > /logs/gps_time_publisher.log 2>&1 & 

# Prevent the container from exiting immediately
tail -f /dev/null