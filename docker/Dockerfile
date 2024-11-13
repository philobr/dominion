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

WORKDIR /dominion
CMD [ "/bin/bash" ]

FROM install AS build

ADD .. /dominion

RUN ls -la

RUN mkdir -p build
WORKDIR /dominion/build
RUN cmake .. && make

CMD [ "./server_exe" ]
