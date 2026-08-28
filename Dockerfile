# 1. Start with a base Linux environment
FROM ubuntu:24.04

# 2. Install compilers, CMake, and SFML dependencies
RUN apt-get update && apt-get install -y --no-install-recommends\
    build-essential \
    cmake \
    git \
    libmbedtls-dev \
    libssh2-1-dev \
    && rm -rf /var/lib/apt/lists/*

# 3. Copy your C++ source code into the container
WORKDIR /app
COPY . .

# 4. Use CMake to build the BlokusServer executable
RUN mkdir build && cd build && \
    cmake .. && \
    cmake --build .

# Run the game as a non-root user
RUN useradd --system --create-home --uid 10001 gameserver \
    && chown -R gameserver:gameserver /app
USER gameserver

# 5. Tell the container to run the compiled game server on startup
CMD ["./build/bin/BlokusServer"]