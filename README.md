# Distributed File Retrieval System

**Author**: Pranav Rattehalli Mahesh (rattehallipranav@gmail.com || pmahesh@depaul.edu)  
**Programming Language**: C++

## Overview
This project is a high-performance distributed file retrieval system built with **C++**, leveraging **gRPC** and **Protocol Buffers** for efficient client-server communication. It supports **multi-client indexing and searching**, optimizing data retrieval and scalability. The system was designed with **concurrency, performance benchmarking, and real-time search capabilities** in mind.

## Requirements

To build and run the project, ensure that you have **GCC 14.x** and **CMake 3.28.x** installed on your system. Additionally, you need to install **Google Protocol Buffers (ProtoBuf)** and **gRPC libraries**.

For **Ubuntu 24.04 LTS**, use the following commands to set up the required dependencies:

```sh
sudo apt install build-essential cmake g++-14 gcc-14 cmake
sudo update-alternatives --remove-all gcc
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 130
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 140
sudo update-alternatives --remove-all g++
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 130
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 140
sudo apt install pkg-config protobuf-compiler libprotobuf-dev protobuf-compiler-grpc libgrpc++-dev
```

---

## Building the Project
### **Compile the Application**
Use the following steps to build the project:

```sh
cd app-cpp
mkdir build
cd build
cmake ..
make
```

---

## Running the Application
### **Start the Server**
After building the project, start the server:

```sh
./file-retrieval-server 
```

**Expected Output:**
```
=== Server Command Menu ===
1. quit/exit - Exit the server application
Server is listening on port 50051
Enter command: 
```

### **Start the Client**
Once the server is running, launch a client:

```sh
./file-retrieval-client 
```

**Expected Output:**
```
> Please enter the server IP address: 127.0.0.1
> Please enter the server port: 50051
Connecting to server 127.0.0.1:50051...
gRPC Client initialized and ready to connect to the server at 127.0.0.1:50051
[INFO] Connected to server with Client ID: 1
Connected to the server successfully.
> Options available: index <Folder path> | search <Terms> | quit
```

---

## Multi-Client Example (2 Clients, 1 Server)
### **Step 1: Start the Server**
```sh
./file-retrieval-server 
```

### **Step 2: Launch Multiple Clients**
#### **Client 1**
```sh
./file-retrieval-client 
```

#### **Client 2**
```sh
./file-retrieval-client 
```

### **Step 3: Index Files from Clients**
#### **Client 1**
```sh
> index ../../TEST/Test
Indexing folder: ../../TEST/Test...
Completed indexing 1007 bytes of data
Completed indexing in 0.085355 seconds
Indexing completed successfully.
```

#### **Client 2**
```sh
> index ../../TEST/Test 2 
Indexing folder: ../../TEST/Test 2...
Completed indexing 721 bytes of data
Completed indexing in 0.0227595 seconds
Indexing completed successfully.
```

---

### **Step 4: Perform Search Queries**
#### **Client 1**
```sh
> search Chicago
Server message: Search completed in 0.000014 seconds. Search results (top 3 out of 3):
ClientID:Document Path: 2:../../TEST/Test 2/TEST 3.txt, Count: 8
ClientID:Document Path: 1:../../TEST/Test/TEST 1.txt, Count: 8
ClientID:Document Path: 1:../../TEST/Test/TEST 3.txt, Count: 8
```

#### **Client 2**
```sh
> search Chicago and India
Server message: Search completed in 0.000018 seconds. Search results (top 3 out of 3):
ClientID:Document Path: 2:../../TEST/Test 2/TEST 3.txt, Count: 12
ClientID:Document Path: 1:../../TEST/Test/TEST 3.txt, Count: 12
ClientID:Document Path: 1:../../TEST/Test/TEST 1.txt, Count: 9
```

---

### **Step 5: Disconnect Clients**
#### **Client 1**
```sh
> quit
Thanks for using the File Retrieval Engine!
```

#### **Client 2**
```sh
> quit
Thanks for using the File Retrieval Engine!
```

### **Step 6: Shut Down the Server**
```sh
quit
Shutting down the server...
Server application exited.
```

---

## Benchmarking with Multiple Clients
This system supports **benchmarking performance** across multiple clients. 

### **Step 1: Start the Server**
```sh
./file-retrieval-server 
```

### **Step 2: Run the Benchmark**
```sh
./file-retrieval-benchmark 
```

**Example Input:**
```
Enter the number of clients: 2
Enter the server IP address: 127.0.0.1
Enter the server port: 50051
Enter the path for dataset 1: ../../TEST/Test
Enter the path for dataset 2: ../../TEST/Test 2
Enter search command: Chicago and India
```

**Expected Output:**
```
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
```

### **Step 3: Shut Down the Server**
```sh
> quit
```

