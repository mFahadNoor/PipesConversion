#include <iostream>
#include <fstream>
#include <thread>
#include <csignal>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <sys/stat.h>  

const char* PIPE_A_TO_B = "pipe_A_to_B";
const char* PIPE_B_TO_A = "pipe_B_to_A";

// Signal handler for SIGUSR1
void signal_handler(int signum) {
    std::cout << "Process A: Received SIGUSR1 from Process B. A processed message is ready to be read." << std::endl;
}

// Message Generator Thread (Process A)
void message_generator() {
    int fd = open(PIPE_A_TO_B, O_WRONLY);
    if (fd == -1) {
        std::cerr << "Process A: Failed to open pipe_A_to_B for writing" << std::endl;
        return;
    }

    while (true) {
        std::string message;
        std::cout << "Enter a message to send to Process B: ";
        std::getline(std::cin, message);

        if (!message.empty()) {
            write(fd, message.c_str(), message.size() + 1);
            std::cout << "Process A: Sent - " << message << std::endl;
            sleep(2);  // Delay for demonstration
        }
    }

    close(fd);
}

// Message Handler Thread (Process A)
void message_handler() {
    int fd = open(PIPE_B_TO_A, O_RDONLY);
    if (fd == -1) {
        std::cerr << "Process A: Failed to open pipe_B_to_A for reading" << std::endl;
        return;
    }

    char buffer[100];
    while (true) {
        ssize_t bytes = read(fd, buffer, sizeof(buffer));
        if (bytes > 0) {
            std::cout << "Process A: Received processed message - " << buffer << std::endl;
        }
    }

    close(fd);
}

// Signal Waiter Thread (Process A)
void signal_waiter() {
    while (true) {
        pause();  // Wait for signal
    }
}

int main() {
    mkfifo("pipe_A_to_B", 0666);
    mkfifo("pipe_B_to_A", 0666);
    // Display Process A's PID
    std::cout << "Process A PID: " << getpid() << std::endl;

    // Register signal handler for SIGUSR1
    signal(SIGUSR1, signal_handler);

    // Start threads in Process A
    std::thread generator(message_generator);
    std::thread handler(message_handler);
    std::thread waiter(signal_waiter);

    // Join threads (never actually exits in this looped example)
    generator.join();
    handler.join();
    waiter.join();

    return 0;
}
