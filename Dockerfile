FROM ubuntu:latest

RUN apt-get -y update && apt-get -y install \
	wget \
	git \
	vim \
	qtbase5-dev \
	cmake \
	g++ \
	qtdeclarative5-dev \
	qtquickcontrols2-5-dev
RUN apt-get -y install gperf

RUN mkdir -p /src

WORKDIR /src
CMD ["bash"]

# cmake && make