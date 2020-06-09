ARG image
FROM $image
# TODO: is it possible to use environment variable here instead? 
WORKDIR /home/ci
COPY --chown=ci:ci . /home/ci/lfp
WORKDIR /home/ci/lfp/build
RUN cmake -DBUILD_SHARED_LIBS=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Release ..
#WORKDIR /home/ci/lfp
#RUN cmake --build build --target install
#WORKDIR /home/ci/lfp/build
RUN make -j4
RUN ctest --output-on-failure
