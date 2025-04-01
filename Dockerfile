FROM debian:latest

RUN apt update && apt upgrade -y && \
			apt-get install -y \
			build-essential \
			clang \
			make \
			php

COPY . .
