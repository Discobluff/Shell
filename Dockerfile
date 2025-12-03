FROM ubuntu
RUN apt-get update -y
RUN apt-get install --yes cmake
RUN apt-get install --yes clang
RUN apt-get install --yes git
RUN apt-get install --yes net-tools
RUN apt-get install --yes psmisc
RUN apt-get install --yes libreadline-dev