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
	python \
	libx11-dev
RUN mkdir -p /qtsrc
WORKDIR /qtsrc
RUN wget https://download.qt.io/archive/qt/5.7/5.7.1/single/qt-everywhere-opensource-src-5.7.1.7z

RUN apt-get -y install p7zip
RUN p7zip -d qt-everywhere-opensource-src-5.7.1.7z

WORKDIR /qtsrc/qt-everywhere-opensource-src-5.7.1
RUN ./configure -qt-xcb -nomake examples -opensource -confirm-license -nomake tests -skip qt3d -skip qtactiveqt \
    -skip qtandroidextras -skip qtcanvas3d -skip qtcharts -skip qtconnectivity -skip qtdatavis3d -skip qtdeclarative \
	-skip qtdoc -skip qtgamepad -skip qtgraphicaleffects -skip qtimageformats -skip qtlocation -skip qtmacextras \
	-skip qtmultimedia -skip qtpurchasing -skip qtquickcontrols -skip qtquickcontrols2 -skip qtscript -skip qtscxml \
	-skip qtsensors -skip qtserialbus -skip qtserialport -skip qtsvg -skip qttools -skip qttranslations -skip qtvirtualkeyboard \
	-skip qtwayland -skip qtwebchannel -skip qtwebengine -skip qtwebsockets -skip qtwebview -skip qtwinextras -skip qtx11extras -skip qtxmlpatterns
RUN make
RUN make install; exit 0
RUN CPATH=/usr/local/Qt-5.7.1/include
RUN LIBRARY_PATH=/usr/local/Qt-5.7.1/lib
RUN CMAKE_PREFIX_PATH=/usr/local/Qt-5.7.1
RUN export CPATH
RUN export LIBRARY_PATH
RUN export CMAKE_PREFIX_PATH

RUN mkdir -p /src
WORKDIR /src
CMD ["bash"]

# cmake && make