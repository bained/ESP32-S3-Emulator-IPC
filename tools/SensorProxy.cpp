#define NOMINMAX
#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

const char* PIPE_NAME = "\\\\.\\pipe\\SensorPipe";

int main() {
    std::cout << "==========================================" << std::endl;
    std::cout << "       Sensor Proxy Tool (IPC Server)     " << std::endl;
    std::cout << "==========================================" << std::endl;

    while (true) {
        std::cout << "\n[Info] Creating Named Pipe: " << PIPE_NAME << "..." << std::endl;

        // Create the Named Pipe
        HANDLE hPipe = CreateNamedPipeA(
            PIPE_NAME,
            PIPE_ACCESS_DUPLEX,                     // Read/Write access
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, // Message type, blocking wait on Connect
            1,                                      // Only 1 instance at a time
            1024,                                   // Out buffer size
            1024,                                   // In buffer size
            0,                                      // Default timeout
            NULL                                    // Default security attributes
        );

        if (hPipe == INVALID_HANDLE_VALUE) {
            std::cerr << "[Error] Failed to create named pipe. Error: " << GetLastError() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            continue;
        }

        std::cout << "[Info] Waiting for emulator to connect..." << std::endl;

        // Wait for the emulator (client) to connect
        BOOL connected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

        if (!connected) {
            std::cerr << "[Error] ConnectNamedPipe failed. Error: " << GetLastError() << std::endl;
            CloseHandle(hPipe);
            continue;
        }

        std::cout << "[Success] Emulator connected!" << std::endl;
        std::cout << "Enter a numeric sensor value (0-4095) to send, or 'exit' to restart pipe:" << std::endl;

        // Thread to asynchronously read command updates back from the emulator
        bool runCommunication = true;
        std::thread readerThread([&]() {
            char readBuf[128];
            while (runCommunication) {
                DWORD bytesAvailable = 0;
                BOOL success = PeekNamedPipe(hPipe, NULL, 0, NULL, &bytesAvailable, NULL);
                if (!success) {
                    std::cout << "\n[Info] Emulator disconnected (Peek failed)." << std::endl;
                    runCommunication = false;
                    break;
                }

                if (bytesAvailable > 0) {
                    DWORD bytesRead = 0;
                    size_t readSize = (bytesAvailable < sizeof(readBuf) - 1) ? bytesAvailable : sizeof(readBuf) - 2;
                    success = ReadFile(hPipe, readBuf, static_cast<DWORD>(readSize), &bytesRead, NULL);
                    if (success && bytesRead > 0) {
                        readBuf[bytesRead] = '\0';
                        std::cout << "\n[Received Command] Emulator sent command: \"" << readBuf << "\"" << std::endl;
                        std::cout << "> " << std::flush;
                    } else if (!success) {
                        std::cout << "\n[Info] Emulator disconnected (Read failed)." << std::endl;
                        runCommunication = false;
                        break;
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });

        // Main input loop
        while (runCommunication) {
            std::cout << "> " << std::flush;
            std::string input;
            if (!std::getline(std::cin, input)) {
                runCommunication = false;
                break;
            }

            if (input == "exit" || input == "quit") {
                runCommunication = false;
                break;
            }

            if (input == "P" || input == "p") {
                uint32_t val = 2; // PIR HIGH (Motion Detected)
                DWORD bytesWritten = 0;
                BOOL success = WriteFile(hPipe, &val, sizeof(val), &bytesWritten, NULL);
                if (success && bytesWritten == sizeof(val)) {
                    std::cout << "[Sent] PIR Sensor - MOTION DETECTED (HIGH)" << std::endl;
                } else {
                    std::cerr << "[Error] Failed to write to pipe." << std::endl;
                    runCommunication = false;
                }
                continue;
            }

            if (input == "O" || input == "o") {
                uint32_t val = 3; // PIR LOW (No Motion)
                DWORD bytesWritten = 0;
                BOOL success = WriteFile(hPipe, &val, sizeof(val), &bytesWritten, NULL);
                if (success && bytesWritten == sizeof(val)) {
                    std::cout << "[Sent] PIR Sensor - NO MOTION (LOW)" << std::endl;
                } else {
                    std::cerr << "[Error] Failed to write to pipe." << std::endl;
                    runCommunication = false;
                }
                continue;
            }

            if (input == "B" || input == "b") {
                uint32_t valPress = 0;   // Pressed (LOW)
                uint32_t valRelease = 1; // Released (HIGH)
                DWORD bytesWritten = 0;
                
                // Simulate physical press (LOW)
                BOOL success = WriteFile(hPipe, &valPress, sizeof(valPress), &bytesWritten, NULL);
                if (success && bytesWritten == sizeof(valPress)) {
                    std::cout << "[Sent] Button - PRESS (LOW)..." << std::flush;
                    
                    // Hold for 150 milliseconds
                    std::this_thread::sleep_for(std::chrono::milliseconds(150));
                    
                    // Simulate physical release (HIGH)
                    success = WriteFile(hPipe, &valRelease, sizeof(valRelease), &bytesWritten, NULL);
                    if (success && bytesWritten == sizeof(valRelease)) {
                        std::cout << " RELEASE (HIGH)" << std::endl;
                    } else {
                        std::cerr << " [Error] Failed to send release." << std::endl;
                        runCommunication = false;
                    }
                } else {
                    std::cerr << "[Error] Failed to send press." << std::endl;
                    runCommunication = false;
                }
                continue;
            }

            if (input == "M" || input == "m") {
                uint32_t val = 0; // Closed (LOW)
                DWORD bytesWritten = 0;
                BOOL success = WriteFile(hPipe, &val, sizeof(val), &bytesWritten, NULL);
                if (success && bytesWritten == sizeof(val)) {
                    std::cout << "[Sent] Reed Switch - Magnet NEAR (Closed / LOW)" << std::endl;
                } else {
                    std::cerr << "[Error] Failed to write to pipe." << std::endl;
                    runCommunication = false;
                }
                continue;
            }

            if (input == "N" || input == "n") {
                uint32_t val = 1; // Open (HIGH)
                DWORD bytesWritten = 0;
                BOOL success = WriteFile(hPipe, &val, sizeof(val), &bytesWritten, NULL);
                if (success && bytesWritten == sizeof(val)) {
                    std::cout << "[Sent] Reed Switch - Magnet AWAY (Open / HIGH)" << std::endl;
                } else {
                    std::cerr << "[Error] Failed to write to pipe." << std::endl;
                    runCommunication = false;
                }
                continue;
            }

            try {
                // Parse and send the number
                uint32_t val = static_cast<uint32_t>(std::stoul(input));
                DWORD bytesWritten = 0;
                BOOL success = WriteFile(
                    hPipe,
                    &val,
                    sizeof(val),
                    &bytesWritten,
                    NULL
                );

                if (success && bytesWritten == sizeof(val)) {
                    std::cout << "[Sent] Successfully sent sensor value: " << val << std::endl;
                } else {
                    std::cerr << "[Error] Failed to write value to pipe." << std::endl;
                    runCommunication = false;
                }
            } catch (const std::exception&) {
                std::cout << "[Warning] Enter valid number, 'P' (PIR Motion), 'O' (PIR No-Motion), 'B' (Button Click), 'M'/'N' (Reed) or 'exit'." << std::endl;
            }
        }

        runCommunication = false;
        if (readerThread.joinable()) {
            readerThread.join();
        }

        std::cout << "[Info] Closing current connection session..." << std::endl;
        DisconnectNamedPipe(hPipe);
        CloseHandle(hPipe);
    }

    return 0;
}
