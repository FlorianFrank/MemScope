# Use a base image with the necessary tools for ARM development
FROM ubuntu:latest

# Install dependencies
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    cmake \
    git \
    gcc-arm-none-eabi \
    gdb-arm-none-eabi \
    libnewlib-arm-none-eabi \
    libstdc++-arm-none-eabi-newlib \
    make \
    python3 \
    python3-pip \
    wget \
    unzip

# Check the installation of essential tools
RUN which cmake && which make && which arm-none-eabi-gcc

# Set up the working directory
WORKDIR /project

# Copy the local project files into the Docker container
COPY . /project

# Ensure the compile script is executable
RUN chmod +x /project/compile_fw_STM32F4.bash

# Run the compile script
RUN /project/compile_fw_STM32F4.bash

# Default command
CMD ["bash"]
