# Use the ROS 2 Jazzy image as the base
FROM ros:jazzy

# Install packages for debugging and such
RUN apt-get update && apt-get install -y \
    nano \
    && rm -rf /var/lib/apt/lists/*

# Copy in the sdk
RUN mkdir -p /ros2_ws
COPY ./ros2_ws /ros2_ws

# Set the working directory to /ros2_ws/src
WORKDIR /ros2_ws/src

# Create a symbolic link to the ROS 2 folder
# RUN ln -s inertial-sense-sdk/ROS/ros2

# Build the workspace
# RUN . /opt/ros/jazzy/setup.sh && \
#     cd /ros2_ws && \
#     colcon build

# Source the setup file
RUN echo 'source /opt/ros/jazzy/setup.sh' >> ~/.bashrc && \
    echo 'source /ros2_ws/install/setup.sh' >> ~/.bashrc
