# Blockchain-Simulator
Developed a multithreaded C/C++ application to simulate a basic cryptocurrency blockchain network. This project focuses on synchronization between consumer and producer threads within the same process, implementing a blockchain ledger managed by four mining threads and one server thread.
# Features
- Blockchain Implementation: Uses blocks that store a cryptographic hash, a timestamp, and transaction data.
- Multithreading Support: Incorporates four mining threads that compute crc32 hashes and a server thread that manages the blockchain.
- Proof of Work: Includes a crc32-based hashing mechanism that requires finding hashes with a predetermined number of leading zeros.
- Adversarial Testing: Features a bad miner designed to test the robustness of the blockchain by submitting blocks with deliberately        incorrect hashes.
- Security and Validation: Implements checks to ensure only valid blocks are added to the blockchain.
- Error Handling: Handles various error scenarios to maintain the integrity and reliability of the blockchain.
# Installing
1.Clone the repo: git clone https://github.com/MrBomi/Blockchain-Simulator
2. Navigate to the project directory: cd Blockchain-Simulator

If using CMake(Added optional CMakeLists File):
  Create a build directory: mkdir build && cd build
  Generate the Makefile with CMake: cmake ..
  Build the project: make
  
Alternatively, follow any standard build process you prefer without CMake.
