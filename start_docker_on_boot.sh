#!/bin/bash

IMU_ROS2_IS_PATH=/home/yride/imu_ros2_IS

# Start the docker container
docker compose -f $IMU_ROS2_IS_PATH/docker_compose.yaml up

# # Prevent the container from exiting immediately
# tail -f /dev/null