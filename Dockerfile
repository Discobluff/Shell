FROM ubuntu
RUN apt-get update -y
RUN apt-get install --yes cmake
RUN apt-get install --yes clang
RUN apt-get install --yes git