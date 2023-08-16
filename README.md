# HISTO-SYSTEM Assignment

Welcome to the HISTO-SYSTEM assignment! Think of this assignment as a puzzle that you need to solve – and in terms of designing your software there is no single right answer – there are many potential correct solutions. Where requirements have not been stated – you need to figure out a way to do what you need to do in order to satisfy the requirements and get the overall job done. This means that a lot of the design choices in this application are left up to you …

## Overview

Create an application suite called HISTO-SYSTEM consisting of three distinct processing components:

1. Data Consumer (DC) application
2. Two different Data Producer (DP-1 and DP-2) applications

You will need to write the Data Consumer and each of the Data Producer programs in ANSI C under Linux. The purpose of this assignment is to help you understand how, why, and when certain IPC mechanisms are used within UNIX/Linux System Application programming. This program will use semaphores, shared memory, and signals. You can build upon the sample code demonstrated and provided in class.

## Considerations

- All applications should have a modular design with module and function comment blocks.
- Comment your code for clarity.
- Organize your code with the required directory structure and makefiles for each component.
- There should be no debugging "print" statements in the final submitted code.

## Data Consumer (DC)

### Purpose

The Data Consumer (DC) program's job is to read data out of the shared memory's circular buffer. The data will be populated in this shared memory space by the two data producers.

### Details

- The circular buffer will hold 256 random letters ('A' to 'T') generated by the producers.
- DC program should check for the existence of shared memory before attempting to read data.
- DC application will be launched from DP-2 and will receive sharedMemoryID, DP-1 processID, and DP-2 processID on the command line.
- DC application will read letters out of the buffer using the reading index.
- DC will keep track of the number of each letter and display a histogram of counts every 10 seconds.
- DC will display the histogram with special symbols for count units.
- If DC receives a SIGINT signal, it will send a SIGINT signal to each producer.
- DC will display the final histogram, clean up IPC, and exit.

## Data Producer(s) (DP-1 and DP-2)

### Purpose

The Data Producer (DP) program's purpose is to set up shared memory for use as a circular buffer, generate random letters ('A' to 'T'), and populate the buffer.

### Details

- DP-1 will allocate shared memory for a circular buffer of 256 characters and launch DP-2, passing the sharedMemoryID to it.
- DP-2 will attach to shared memory and launch the DC application, passing required information.
- DP-1 and DP-2 will guard their writing into the circular buffer using a semaphore.
- DP-1 will generate 20 random letters and write them into the buffer every 2 seconds.
- DP-2 will generate a single random letter and write it every 1/20 of a second.
- DP-1 and DP-2 can handle a SIGINT signal, releasing shared memory and exiting when caught.
- DP-1 and DP-2 will wrap the buffer index back to zero once it reaches 256.

---

**Note**: This README provides an overview of the HISTO-SYSTEM assignment. Please refer to the complete assignment document for detailed instructions and implementation.

For questions and clarifications, reach out to the assignment instructor.
