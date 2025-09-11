FROM ubuntu:25.10

RUN apt-get update && apt-get install -y \
    build-essential \
    clang \
    gdb \
    clangd \
    cmake \
    libstdc++-15-dev \
    git \
    make \
    && rm -rf /var/lib/apt/lists/*
