# Use an official Debian Bookworm as a parent image
FROM debian:bookworm

# Set the working directory in the container
WORKDIR /usr/src/app

# Install dependencies
RUN apt-get update && \
    apt-get install -y build-essential doxygen git && \
    rm -rf /var/lib/apt/lists/*

# Clone your public GitHub repository
RUN git clone https://github.com/jylialho/ctutorial.git .

# Define environment variable
ENV DOCKER=true

# Run command to build your project
CMD ["make"]
