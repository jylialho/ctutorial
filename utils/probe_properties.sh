#!/bin/sh

# Probe control property fields from 0 to 255 with minimal netcat delay, 
# check the responses from the server logs to see which ones are supported
i=0
while [ $i -le 255 ]; do
  # Convert the loop counter to a two-digit hexadecimal number
  hex=$(printf '%02x' $i)

  # Construct the command with the incrementing last byte as the property identifier
  echo -n -e "\x00\x01\x00\x01\x00\x$hex" | nc -u -w1 localhost 4000

  # Increment the counter
  i=$((i + 1))
done
