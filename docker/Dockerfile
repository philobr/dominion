FROM ubuntu:22.04 AS install

# Install dependencies
RUN apt-get update && apt-get install -y \
    clang \
    clang-format \
    cmake \
    cppcheck \
    git \
    libwxgtk3.0-gtk3-dev \
    make \
    vim

# This is necessary since the container is run as root while the dominion directory is owned by the host user
RUN git config --global --add safe.directory /dominion

WORKDIR /dominion

CMD [ "/bin/bash" ]

FROM install AS build

ADD .. /dominion

RUN ls -la

RUN mkdir -p build
WORKDIR /dominion/build
RUN cmake .. && make

CMD [ "./server_exe" ]
