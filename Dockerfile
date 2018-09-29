FROM ubuntu:latest
ENV PKG_CONFIG_PATH=/usr/local/lib/pkgconfig
RUN mkdir -p /chronicle-build
#WORKDIR /chronicle-build
COPY ./ ./chronicle-build/
VOLUME /chronicle-build
RUN /chronicle-build/.ci/mingw-64/apt-script.sh
#COPY ./spdlog/ ./spdlog
#COPY ./lame/ ./lame
#COPY ./*.h ./
#COPY ./*.cpp ./
#COPY ./aclocal.m4 .
#COPY ./autogen.sh .
#COPY ./.ci/mingw-64/* ./.ci/mingw-64/
