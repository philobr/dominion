FROM ubuntu:22.04 AS install

ARG UID=1000
ARG GID=1000

RUN addgroup --gid $GID dominion && \
    adduser --uid $UID --gid $GID dominion --disabled-password && \
    echo "dominion ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

# Install dependencies
RUN apt-get update
RUN apt-get install -y \
    clang \
    clang-format \
    clang-tidy \
    cmake \
    cppcheck \
    git \
    libwxgtk3.0-gtk3-dev \
    make \
    vim

USER dominion

# This is necessary since the container is run as root while the dominion directory is owned by the host user
RUN git config --global --add safe.directory /dominion

WORKDIR /dominion

CMD [ "/bin/bash" ]

FROM install AS build

ADD .. /dominion

RUN mkdir -p build
WORKDIR /dominion/build
RUN cmake .. && make -j $(nproc)

CMD [ "./server_exe" ]
