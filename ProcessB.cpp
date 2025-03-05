#include <iostream>
#include <csignal>
#include <unistd.h>
#include <fcntl.h>  // For open(), O_RDONLY, O_WRONLY
#include <string>

const char* PIPE_A_TO_B = "pipe_A_to_B";
const char* PIPE_B_TO_A = "pipe_B_to_A";

// Signal sending function to Process A
void send_signal_to_processA(pid_t processA_pid) {
    kill(processA_pid, SIGUSR1);
}

// Message Reader and Processor (Process B)
void message_reader_and_processor(pid_t processA_pid) {
    int fd_in = open(PIPE_A_TO_B, O_RDONLY);
    int fd_out = open(PIPE_B_TO_A, O_WRONLY);
    
    if (fd_in == -1 || fd_out == -1) {
        std::cerr << "Process B: Failed to open pipes" << std::endl;
        return;
    }

    char buffer[100];
    while (true) {
        ssize_t bytes = read(fd_in, buffer, sizeof(buffer));
        if (bytes > 0) {
            std::string message(buffer);
            std::cout << "Process B: Received - " << message << std::endl;
            
            // Process the message (e.g., convert to uppercase)
            for (char& c : message) c = toupper(c);

            // Send processed message back to Process A
            write(fd_out, message.c_str(), message.size() + 1);
            std::cout << "Process B: Processed and sent - " << message << std::endl;

            // Send signal to Process A to notify that message is ready
            send_signal_to_processA(processA_pid);
            sleep(1);  // Delay for demonstration
        }
    }

    close(fd_in);
    close(fd_out);
}

int main() {
    pid_t processA_pid;
    std::cout << "Enter Process A PID to send signals: ";
    std::cin >> processA_pid;

    message_reader_and_processor(processA_pid);

    return 0;
}

