FROM ubuntu:20.04

# install non-interactive
ENV DEBIAN_FRONTEND=noninteractive

# install required deps
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    wget \
    g++ \
    gdb \
    gcc \
    ninja-build \
    git \
    software-properties-common \
    tar \
    && rm -rf /var/lib/apt/lists/*

RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    gnupg \
    wget \
    lsb-release

RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null
RUN echo "deb https://apt.kitware.com/ubuntu/ $(lsb_release -cs) main" | tee /etc/apt/sources.list.d/kitware.list > /dev/null

# refresh
RUN apt-get update

# install
RUN apt-get install -y --no-install-recommends \
    cmake \
    libavcodec-dev \
    libavformat-dev \
    libswscale-dev
RUN rm -rf /var/lib/apt/lists/*


# verify
RUN cmake --version