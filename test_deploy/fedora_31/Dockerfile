# Set ubuntu as base image
FROM fedora:31

# Install dependencies
#RUN apt-get update && apt-get install -y cmake gcc g++ git qt5-default
RUN dnf -y upgrade && dnf -y install cmake make gcc gcc-c++ git qt5-devel

WORKDIR /home/build

CMD ["bash"]

