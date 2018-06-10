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
									libx11-dev \
									p7zip \
									nodejs \
	&& mkdir -p /qtsrc \
	&& cd /qtsrc \
    && wget https://download.qt.io/archive/qt/5.7/5.7.1/single/qt-everywhere-opensource-src-5.7.1.7z \
	&& p7zip -d qt-everywhere-opensource-src-5.7.1.7z \
	&& cd /qtsrc/qt-everywhere-opensource-src-5.7.1 \
	&& ./configure -qt-xcb -nomake examples -opensource -confirm-license -nomake tests -skip qt3d -skip qtactiveqt \
    -skip qtandroidextras -skip qtcanvas3d -skip qtcharts -skip qtconnectivity -skip qtdatavis3d -skip qtdeclarative \
	-skip qtdoc -skip qtgamepad -skip qtgraphicaleffects -skip qtimageformats -skip qtlocation -skip qtmacextras \
	-skip qtmultimedia -skip qtpurchasing -skip qtquickcontrols -skip qtquickcontrols2 -skip qtscript -skip qtscxml \
	-skip qtsensors -skip qtserialbus -skip qtserialport -skip qtsvg -skip qttools -skip qttranslations -skip qtvirtualkeyboard \
	-skip qtwayland -skip qtwebchannel -skip qtwebengine -skip qtwebsockets -skip qtwebview -skip qtwinextras -skip qtx11extras -skip qtxmlpatterns \
	&& make -j 10 \
	&& make install \
	; CPATH=/usr/local/Qt-5.7.1/include \
    && LIBRARY_PATH=/usr/local/Qt-5.7.1/lib \
    && CMAKE_PREFIX_PATH=/usr/local/Qt-5.7.1 \
    && export CPATH \
    && export LIBRARY_PATH \
    && export CMAKE_PREFIX_PATH \
	&& mkdir -p /src \
	&& rm -r /qtsrc

RUN apt-get -y update && apt-get -y install screen

WORKDIR /src
CMD ["bash"]
