FROM ubuntu:mantic

RUN set -ex;                                                                      \
    apt-get update;                                                               \
    apt-get install -y libpng-dev libsfml-dev libuv1-dev;                              \
    mkdir -p /usr/src;                                                            

COPY ./build/server /usr/src/
COPY ./resources /usr/src/resources/
WORKDIR /usr/src

EXPOSE 2001:2001
ENTRYPOINT ["./server"]