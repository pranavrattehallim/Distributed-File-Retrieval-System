## CSC 435 Programming Assignment 5 (Fall 2024)
**Jarvis College of Computing and Digital Media - DePaul University**

**Student**: Pranav Rattehalli Mahesh (rattehallipranav@gmail.com || pmahesh@depaul.edu)  
**Solution programming language**: C++

### Requirements

If you are implementing your solution in C++ you will need to have GCC 14.x and CMake 3.28.x installed on your system. You will also need to install Google Protocol Buffers (ProtoBuf) and gRPC libraries and development files. On Ubuntu 24.04 LTS you can install GCC and set it as default compiler using the following commands:

```
sudo apt install build-essential cmake g++-14 gcc-14 cmake
sudo update-alternatives --remove-all gcc
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 130
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 140
sudo update-alternatives --remove-all g++
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 130
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 140
sudo apt install pkg-config protobuf-compiler libprotobuf-dev protobuf-compiler-grpc libgrpc++-dev

```

### C++ solution
#### How to build/compile

To build the C++ solution use the following commands:
```
cd app-cpp
mkdir build
cd build
cmake ..
make
```

#### How to run application

To run the C++ server (after you build the project) use the following command:
```
./file-retrieval-server 

=== Server Command Menu ===
1. quit/exit - Exit the server application
Enter command: Server is listening on port 50051
```

To run the C++ client (after you build the project) use the following command:
```
./file-retrieval-client 
> Please enter the server IP address: 127.0.0.1
> Please enter the server port: 50051
Connecting to server 127.0.0.1:50051...
gRPC Client initialized and ready to connect to the server at 127.0.0.1:50051
[INFO] Connected to server with Client ID: 1
Connected to the server successfully.
> Options available: index <Folder path> | search <Terms> | quit
```

#### Example (2 clients and 1 server)

**Step 1:** start the server:

Server
```
./file-retrieval-server 

=== Server Command Menu ===
1. quit/exit - Exit the server application
Server is listening on port 50051
Enter command: 
```

**Step 2:** start the clients and connect them to the server:

Client 1
```
./file-retrieval-client 
> Please enter the server IP address: 127.0.0.1
> Please enter the server port: 50051
Connecting to server 127.0.0.1:50051...
gRPC Client initialized and ready to connect to the server at 127.0.0.1:50051
[INFO] Connected to server with Client ID: 1
Connected to the server successfully.
> Options available: index <Folder path> | search <Terms> | quit
>
```

Client 2
```
./file-retrieval-client 
> Please enter the server IP address: 127.0.0.1
> Please enter the server port: 50051
Connecting to server 127.0.0.1:50051...
gRPC Client initialized and ready to connect to the server at 127.0.0.1:50051
[INFO] Connected to server with Client ID: 2
Connected to the server successfully.
> Options available: index <Folder path> | search <Terms> | quit
> 
```

**Step 3:** index files from the clients:

Client 1
```
> Options available: index <Folder path> | search <Terms> | quit
> index ../../TEST/Test
Indexing folder: ../../TEST/Test...
Completed indexing 1007 bytes of data
Completed indexing in 0.085355 seconds
Indexing completed successfully.
```

Client 2
```
> Options available: index <Folder path> | search <Terms> | quit
> index ../../TEST/Test 2 
Indexing folder: ../../TEST/Test 2...
Completed indexing 721 bytes of data
Completed indexing in 0.0227595 seconds
Indexing completed successfully.
```

**Step 4:** search files from the clients:

Client 1
```
> Options available: search <Terms> | quit
> search at
Server message: Search completed in 0.000016 seconds. Search results (top 0 out of 0):
> Options available: search <Terms> | quit
> search Chicago
Server message: Search completed in 0.000014 seconds. Search results (top 3 out of 3):
ClientID:Document Path: 2:../../TEST/Test 2/TEST 3.txt, Count: 8
ClientID:Document Path: 1:../../TEST/Test/TEST 1.txt, Count: 8
ClientID:Document Path: 1:../../TEST/Test/TEST 3.txt, Count: 8
```

Client 2
```
> Options available: search <Terms> | quit
> search Chicago and India
Server message: Search completed in 0.000018 seconds. Search results (top 3 out of 3):
ClientID:Document Path: 2:../../TEST/Test 2/TEST 3.txt, Count: 12
ClientID:Document Path: 1:../../TEST/Test/TEST 3.txt, Count: 12
ClientID:Document Path: 1:../../TEST/Test/TEST 1.txt, Count: 9
> quit
```

**Step 5:** close and disconnect the clients:

Client 1
```
> Options available: search <Terms> | quit
> quit
Thanks for using the File Retrieval Engine!
```

Client 2
```
> Options available: search <Terms> | quit
> quit
Thanks for using the File Retrieval Engine!
```

**Step 6:** close the server:

Server
```
quit
Shutting down the server...
Server application exited.
```

#### Example (benchmark with 2 clients and 1 server)

**Step 1:** start the server:

Server
```
./file-retrieval-server 

=== Server Command Menu ===
1. quit/exit - Exit the server application
Server is listening on port 50051
Enter command: 
```

**Step 2:** start the benchmark:

Benchmark
```
./file-retrieval-benchmark 
Enter the number of clients: 2
Enter the server IP address: 127.0.0.1
Enter the server port: 50051
Enter the path for dataset 1: ../../TEST/Test
Enter the path for dataset 2: ../../TEST/Test 2
Enter search command: Chicago and India
gRPC Client initialized and ready to connect to the server at 127.0.0.1:50051
[INFO] Connected to server with Client ID: 1
gRPC Client initialized and ready to connect to the server at 127.0.0.1:50051
[INFO] Connected to server with Client ID: 2
Completed indexing 1007 bytes of data
Completed indexing in 0.00715485 seconds
Client finished indexing: ../../TEST/Test
Completed indexing 721 bytes of data
Completed indexing in 0.00257648 seconds
Client finished indexing: ../../TEST/Test 2
Chicago and India 
Server message: Search completed in 0.000018 seconds. Search results (top 3 out of 3):
ClientID:Document Path: 2:../../TEST/Test 2/TEST 3.txt, Count: 12
ClientID:Document Path: 1:../../TEST/Test/TEST 3.txt, Count: 12
ClientID:Document Path: 1:../../TEST/Test/TEST 1.txt, Count: 9
```

**Step 3:** close the server:

Server
```
> quit
```

