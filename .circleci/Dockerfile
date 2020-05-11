FROM s390x/debian
RUN useradd --create-home --shell /bin/bash ci
WORKDIR /home/ci
COPY --chown=ci:ci . /home/ci/lfp
RUN apt-get update
RUN apt-get install -y cmake g++ git libfmt-dev
WORKDIR /home/ci/lfp/build
RUN cmake -DBUILD_SHARED_LIBS=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Release ..
WORKDIR /home/ci/lfp
RUN cmake --build build --target install
WORKDIR /home/ci/lfp/build
RUN ctest --output-on-failure