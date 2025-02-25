# Use the ROS 2 $ROS_DISTRO image as the base
ARG ROS_DISTRO=jazzy

FROM ros:$ROS_DISTRO

# Set the working directory
WORKDIR /ros2_ws/src

# Install necessary packages including Python 3, pip, and virtualenv
RUN apt-get update && apt-get install -y \
    unzip \
    zip \
    libudev-dev \
    libboost-all-dev \
    python3 \
    python3-pip \
    python3-venv \
    && rm -rf /var/lib/apt/lists/*

# Copy the local repository from the host into the container
WORKDIR /ros2_ws/src
COPY ./ros2_ws/src/inertial-sense-sdk ./inertial-sense-sdk
COPY ./ros2_ws/src/gps_time_publisher ./gps_time_publisher

# Install sdk dependencies
RUN cd ./inertial-sense-sdk/scripts && \
    ./build_ros2.sh

# Source the setup file
RUN echo 'source /opt/ros/${ROS_DISTRO}/setup.bash' >> ~/.bashrc && \
    echo 'source /ros2_ws/install/setup.bash' >> ~/.bashrc

# Copy in the bash script to start the ros2 topics and set the entry point 
COPY ./start_ros2_in_docker.sh /start_ros2_in_docker.sh
RUN chmod +x /start_ros2_in_docker.sh
ENTRYPOINT [ "/start_ros2_in_docker.sh" ]

# Default command to open a bash shell
CMD ["/bin/bash"]
