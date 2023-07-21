# Watchdog
## Overview
This repository contains a C-based watchdog project that provides a mechanism to monitor the execution of a process and restart it in case of failures or crashes. The project consists of several files with different functionalities, including heap management, priority queue, task scheduling, utility functions, unique ID generation, and the main watchdog implementation.

## Components
The WatchDog project consists of the following components:
1. **API Functions:**
* **startWD:** The user includes this function in the code segment of their program that they wish to monitor. When called, it initiates the watchdog process and begins the monitoring procedure.
*  **stopWD:** This function allows the user to stop the watchdog process and terminate the monitoring.

2. **Shared Library:**
The shared library contains various modules and functionalities developed during the course, necessary to support the WatchDog operation. These modules assist in signal handling, task scheduling, and other core functionalities.

3. **WatchDog Executable:**
The WatchDog executable is provided to the user. When the startWD function is called, it forks and executes the WatchDog process, creating a new process responsible for monitoring the user's program.

## Process Flow
1. **WatchDog Initialization:**
* When the user program calls the ```startWD``` function, a new process is created by forking the user program.
*The newly forked process then executes the WatchDog executable using the exec system call, launching the WatchDog process independently.

2. **Thread Creation and Signal Handling:**
* Both the user program and the WatchDog process open separate threads for their respective functionalities.
* Signal handling is implemented to facilitate communication between the user program and the WatchDog process. The user program sends periodic signals to the WatchDog process to indicate that it is running successfully.

3. **Monitoring the User's Program:**
* The WatchDog process continuously checks for signals from the user program. If it receives signals within the expected interval, it assumes that the user program is running correctly and resets the signal counter.
* If the WatchDog process stops receiving signals, it indicates a potential failure in the user program. The WatchDog incrementally counts the number of missed signals, comparing them against a predefined threshold.

4. **Failure Detection and Recovery:**
* If the number of missed signals exceeds the threshold, the WatchDog concludes that the user program has failed or crashed.
* Depending on the execution path, the WatchDog process will initiate the recovery procedure:
** If the WatchDog process is monitoring the user program and detects a failure, it forks and executes a new WatchDog process to monitor the user program again.
** If the user program is being monitored and fails, the WatchDog process will execute a new user program process, effectively restarting the user program with the provided executable file.

5. **Stopping WatchDog:**
The user program can call the ```stopWD``` function to stop the WatchDog process, terminating the monitoring and releasing any allocated resources.

## Advantages
The WatchDog project offers several benefits to users:

* **Automatic program recovery:** If either the user program or the WatchDog process crashes, the monitoring system ensures a swift restart.
* **User-friendly API:** The ```startWD``` and ```stopWD``` functions provide a simple interface for users to incorporate WatchDog monitoring into their programs.
* **Synchronization and Signal Handling:** By using threads and signals, the project effectively synchronizes the communication between the user program and WatchDog process, making monitoring efficient.

## Project Structure
The project is organized with the following files:

1. ```heap.c``` and ```heap.h```: Implement a generic binary heap data structure used for task scheduling.
2. ```makefile```: Contains the build instructions for the project.
3. ```pq_heap.c``` and ```pq_heap.h```: Define a priority queue data structure implemented using the binary heap.
4. ```scheduler_heap.c``` and ```scheduler_heap.h```: Provide task scheduling functionality based on the priority queue.
5. ```task.c``` and ```task.h```: Define the task structure and associated functions for task management.
6. ```tools.c``` and ```tools.h```: Contain utility functions used across the project.
7. ```uid.c``` and ```uid.h```: Define a unique ID generation mechanism.
8. ```user_main.c```: The user's main program that utilizes the watchdog API to register tasks and start the watchdog.
9. ```vector.c``` and ```vector.h```: Implement a dynamic array data structure.
10.```watchdog.c``` and ```watchdog.h```: Implement the watchdog mechanism for process monitoring and restarting.
11. ```wd_process.c```: Defines the watchdog process, its initialization, and internal functions.

## API Functions
The project provides the following API functions to interact with the watchdog:

### WDStart
```int WDStart(int argc, char **argv, size_t interval, unsigned int threshold);```

**Description:**
This function is used to start the watchdog. It must be called by the user's main program, passing the command-line arguments (argc and argv), the monitoring interval in seconds (interval), and the failure threshold (threshold).

**Parameters:**
* argc: The number of command-line arguments.
* argv: An array of strings containing command-line arguments.
* interval: The monitoring interval in seconds. The watchdog will check the registered tasks periodically with this frequency.
* threshold: The failure threshold. If the watchdog detects that a task has not been executed for threshold consecutive monitoring intervals, it will be considered failed, and the watchdog will attempt to restart it.

**Returns:**
* 0 on successful watchdog initialization.
* 1 if watchdog initialization fails.


### WDStop
```int WDStop(void);```

**Description:**
This function is used to stop the watchdog and free any allocated resources. It should be called when the user's main program is exiting.

**Returns:**
* 0 on successful watchdog termination.
* 1 if watchdog termination fails.

## Building and Running the Project
To build the WatchDog project, use the provided ```makefile``` in the project directory. Simply open a terminal, navigate to the project folder, and execute the following command:
```make```

This will compile the project and generate the executable ```wd_process.out```. You can then run the user process with the watchdog functionality using the following command:
```./user_main [command-line arguments]```
Replace ```[command-line arguments]``` with any arguments your user process requires. The watchdog will monitor the user process based on the provided interval and threshold.

## Contributors
This project was developed by Lin Weintraub. For any questions or contributions, please contact the author.
