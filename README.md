# kernel-mode-ram-read-write

## Overview
**kernel-mode-ram-read-write** is a kernel-mode driver and user-mode application designed to allow direct reading and writing of process memory. 
By utilizing kernel-level operations, this avoids detection by anti-cheats that monitor user-mode memory access functions such as `ReadProcessMemory`, `WriteProcessMemory`, and `OpenProcess`. 
The driver operates by accepting custom Ioctl requests, which enable controlled access to a target process's memory, bypassing common detection mechanisms employed by modern anti-cheat software. 
This makes it particularly useful for scenarios requiring stealthy memory manipulation.

## Disclaimer
**kernel-mode-ram-read-write** is intended for educational purposes only. 
Use responsibly and ensure compliance with all relevant laws and regulations regarding software manipulation and anti-cheat measures. 
Misuse of this software may result in account bans or legal consequences. 
It is strongly advised to test drivers on a virtual machine, and the author assumes no responsibility for any misuse or damage resulting from the use of this software.

## Knowledge
### IRQL (Interrupt Request Level)
**IRQL** stands for **Interrupt Request Level**, which is a crucial concept in Windows kernel programming. 
It represents the priority level at which a particular piece of code is running. 
The Windows operating system uses IRQLs to manage the execution of different processes and interrupts, ensuring that higher-priority tasks can preempt lower-priority ones. 
Understanding IRQL is essential for kernel-mode development because many kernel functions and operations, including memory access and interrupt handling, depend on the current IRQL. 
For instance, when performing operations like reading or writing memory, the driver must operate at the appropriate IRQL to ensure that it does not violate system stability or security.

### IOCTL (Input/Output Control)
**IOCTL** stands for **Input/Output Control** and is a system call used to communicate with device drivers in the Windows operating system. 
IOCTL allows user-mode applications to send control codes to a driver, which can then perform specific functions that are not possible through standard read and write operations.
Enables secure data transfer between user-mode applications and kernel-mode drivers. IOCTL can handle complex data structures and parameters, making it suitable for sophisticated operations beyond simple data reads and writes.
