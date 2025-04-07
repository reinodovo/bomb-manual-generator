# Use an Alpine Linux base image
FROM alpine:3.15 as builder

RUN apk update && apk upgrade && apk add --no-cache \
    python3 \
    py3-pip \
    g++ \
    gcc \
    libstdc++6 \
    git

# Install pip
RUN python3 -m ensurepip
RUN python3 -m pip install --upgrade pip

# Install PlatformIO using pip
RUN pip install platformio

# Set a working directory (optional)
WORKDIR /app

COPY ./modules.txt /app/modules.txt
COPY ./platformio.ini /app/platformio.ini
COPY ./include /app/include
COPY ./src /app/src

RUN pio run -e manual

RUN mkdir -p /app/modules && \
    while read repo; do \
        git clone --depth=1 "$repo" /app/modules/$(basename "$repo" .git) && \
        cd /app/modules/$(basename "$repo" .git) && \
        pio run -e manual || exit 1; \
    done < /app/modules.txt

FROM alpine:3.15

# Install necessary packages (Python, Node.js, npm, curl)
RUN apk update && apk upgrade && apk add --no-cache \
    python3 \
    py3-pip \
    git
RUN apk add --no-cache \
    ttf-freefont

# Install pip
RUN python3 -m ensurepip
RUN python3 -m pip install --upgrade pip

RUN pip install flask

# Set a working directory (optional)
WORKDIR /app

COPY --from=builder /app/.pio /app/.pio
COPY --from=builder /app/modules /app/modules

COPY ./templates /app/templates
COPY ./web /app/web
COPY ./server.py /app/server.py

# Define the default command to execute when the container starts
CMD ["python3", "server.py"]

