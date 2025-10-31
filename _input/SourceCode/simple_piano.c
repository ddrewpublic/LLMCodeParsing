// File: simple_piano.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <signal.h>

#define SAMPLE_RATE 44100
#define AMPLITUDE 32767
#define PI 3.14159265358979323846

// Function to generate a sine wave for a given frequency and duration
void generate_sine_wave(double frequency, double duration, short* buffer) {
    int samples = (int)(SAMPLE_RATE * duration);
    for (int i = 0; i < samples; i++) {
        buffer[i] = (short)(AMPLITUDE * sin(2 * PI * frequency * i / SAMPLE_RATE));
    }
}

// Function to play sound using the system's audio output (using /dev/audio)
void play_sound(short* buffer, int samples) {
    int audio_fd = open("/dev/audio", O_WRONLY);
    if (audio_fd < 0) {
        perror("Unable to open audio device");
        exit(EXIT_FAILURE);
    }
    write(audio_fd, buffer, samples * sizeof(short));
    close(audio_fd);
}

// Function to set terminal to raw mode for capturing keyboard input
void set_raw_mode() {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, &new_termios);
    new_termios.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

// Function to restore terminal to original mode
void restore_terminal(struct termios* original) {
    tcsetattr(STDIN_FILENO, TCSANOW, original);
}

// Signal handler for cleanup on exit
void signal_handler(int sig) {
    restore_terminal(NULL);
    exit(0);
}

// Main function
int main() {
    // Register signal handler for cleanup
    signal(SIGINT, signal_handler);
    
    // Set terminal to raw mode
    struct termios original;
    tcgetattr(STDIN_FILENO, &original);
    set_raw_mode();

    // Frequency mapping for keys
    double frequencies[7] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88}; // C4 to B4
    char keys[7] = {'a', 's', 'd', 'f', 'g', 'h', 'j'}; // Key mappings

    // Buffer for audio samples
    short buffer[SAMPLE_RATE]; // 1-second buffer

    printf("Press keys (a, s, d, f, g, h, j) to play notes. Press Ctrl+C to exit.\n");

    while (1) {
        char ch = getchar(); // Read a character from keyboard input
        for (int i = 0; i < 7; i++) {
            if (ch == keys[i]) {
                generate_sine_wave(frequencies[i], 1.0, buffer); // Generate 1 second of sound
                play_sound(buffer, SAMPLE_RATE); // Play the sound
                break;
            }
        }
    }

    // Restore terminal settings before exiting
    restore_terminal(&original);
    return 0;
}
