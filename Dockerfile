FROM ubuntu:latest

RUN apt-get -y update && apt-get -y install \
	wget \
	git \
	vim \
	cmake \
	g++ \
	gperf \
	valgrind \
	libxcb1-dev \
	python

RUN apt-get -y install libx11-dev

RUN mkdir -p /src
WORKDIR /src
CMD ["bash"]

# cmake && make