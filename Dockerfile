
FROM gcc:latest

WORKDIR /

COPY . .

RUN g++ -o myapp ./src/main.cpp

CMD ["./myapp"]
