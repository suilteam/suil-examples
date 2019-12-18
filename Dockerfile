FROM suilteam/suil-prod:unstable
MAINTAINER "Carter Mbotho <carter@suilteam.com>"
# User to use for building
RUN adduser -s /bin/sh -h /home/build/ -D build
# copy examples build folder
COPY artifacts /home/build/examples
ENV PATH="/home/build/examples/bin:${PATH}"

WORKDIR /home/build