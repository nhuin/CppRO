FROM ubuntu:20.04

WORKDIR /app

RUN apt update -y && \
        DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt install -y --no-install-recommends wget lsb-release wget software-properties-common gnupg python3-pip make git && \
        wget https://apt.llvm.org/llvm.sh && \
        chmod +x llvm.sh && \
        ./llvm.sh 14 && \
        python3 -m pip install cmake conan && \
        conan profile new default --detect
