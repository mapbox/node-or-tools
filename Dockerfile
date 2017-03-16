FROM ubuntu:16.04

WORKDIR /usr/local/src/app
COPY .  /usr/local/src/app

RUN apt-get update && \
    apt-get install -y curl python make g++ && \
    \
    (curl -o- https://raw.githubusercontent.com/creationix/nvm/v0.33.1/install.sh | bash) && \
    export NVM_DIR="$HOME/.nvm" && \
    . ${NVM_DIR}/nvm.sh && \
    nvm install 4.8 && \
    nvm use 4.8 && \
    \
    npm install --unsafe-perm && \
    npm test && \
    \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*
