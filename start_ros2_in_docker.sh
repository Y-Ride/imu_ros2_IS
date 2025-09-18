#!/bin/bash

# Make log files if they don't exist
LOG_DIR=/log_imu
mkdir -p ${LOG_DIR}

# Source ROS 2 setup
source /opt/ros/${ROS_DISTRO}/setup.bash
source /ros2_ws/install/setup.bash

# Run ROS 2 launch commands inside the container
# exec /ros2_ws/src/run_inertial_sense_node.sh > ${LOG_DIR}/inertial_sense_ros2_node.log 2>&1 &
exec /ros2_ws/src/run_inertial_sense_node.sh 2>&1 | tee -a ${LOG_DIR}/inertial_sense_ros2_node.log
# ros2 launch inertial_sense_ros2 inertial_sense_ros2.launch.py > ${LOG_DIR}/inertial_sense_ros2_node.log 2>&1 &
# Integrated gps_time_publisher into the inertial sense sdk so the following is no longer needed
# ros2 run gps_time_publisher gps_time_publisher > ${LOG_DIR}/gps_time_publisher.log 2>&1 &

# Prevent the container from exiting immediately
tail -f /dev/null
