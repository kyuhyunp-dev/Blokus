# 1. Start with a base Linux environment
FROM ubuntu:24.04

# 2. Install compilers, CMake, and SFML dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libfreetype6-dev \
    libxrandr-dev \
    libxcursor-dev \
    libxi-dev \
    libudev-dev \
    libopengl-dev \
    libflac-dev \
    libvorbis-dev \
    libgl1-mesa-dev \
    && rm -rf /var/lib/apt/lists/*

# 3. Copy your C++ source code into the container
WORKDIR /app
COPY . .

# 4. Use CMake to build the BlokusServer executable
RUN mkdir build && cd build && \
    cmake .. && \
    cmake --build .

# 5. Tell the container to run the compiled game server on startup
CMD ["./build/bin/BlokusServer"]