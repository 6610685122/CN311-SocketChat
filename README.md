# CN311-SocketChat

A real-time terminal chatroom for CN311 Operating Systems course.

## Features
- Multi-threaded Server & Client
- Real-time Broadcast
- HW2 Logic: Identity printing (4 threads) and counting competition (16 threads).
- HW4 Logic: Year conversion (BE to AD).

## Setup (WSL)
1. Install build tools:
   ```bash
   sudo apt update && sudo apt install build-essential
   ```
2. Compile:
   ```bash
   make
   ```
3. Run Server:
   ```bash
   ./server
   ```
4. Run Client (in another terminal):
   ```bash
   ./client
   ```

## Commands
- `/hw2`: Run Homework 2 thread logic.
- `/hw4 [Name] [Surname] [BE_Year]`: Run Homework 4 year conversion logic.
- `/exit`: Exit the chat.
