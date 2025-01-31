# Use the ROS 2 Jazzy image as the base
FROM ros:jazzy

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

# Set up the Python virtual environment
# WORKDIR /venv
# RUN python3 -m venv /venv && \
#     /venv/bin/pip install --upgrade pip

# Copy the local repository from the host into the container
WORKDIR /ros2_ws/src
COPY ./ros2_ws/src/inertial-sense-sdk ./inertial-sense-sdk

# Install sdk dependencies
RUN cd ./inertial-sense-sdk/scripts && \
    # mkdir -p .venv && \
    python3 -m venv .venv && \
    . .venv/bin/activate && \
    pip install pyyaml

RUN cd inertial-sense-sdk/scripts && \
    ./install_sdk_dependencies.sh

# Create a symbolic link to the ROS 2 folder
RUN ln -s inertial-sense-sdk/ROS/ros2

# Build the workspace
RUN . /opt/ros/jazzy/setup.sh && \
    cd /ros2_ws && \
    colcon build

# Source the setup file
RUN echo 'source /opt/ros/jazzy/setup.bash' >> ~/.bashrc && \
    echo 'source /ros2_ws/install/setup.bash' >> ~/.bashrc

# Default command to open a bash shell
CMD ["/bin/bash"]
