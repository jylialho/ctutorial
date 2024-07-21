# Use an official Debian Bookworm as a parent image
FROM debian:bookworm

# Set the working directory in the container
WORKDIR /usr/src/app

# Install dependencies
RUN apt-get update && \
    apt-get install -y build-essential doxygen git openssh-client && \
    rm -rf /var/lib/apt/lists/*

# Define environment variable
ENV DOCKER=true

# Create SSH directory
RUN mkdir -p /root/.ssh && \
    ssh-keyscan github.com >> /root/.ssh/known_hosts

# Run command to build your project
CMD ["make"]
