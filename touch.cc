#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
using std::cout;
using std::endl;
using std::string;
#include <linux/input.h>
/*
 *
 * https://stackoverflow.com/questions/28841139/how-to-get-coordinates-of-touchscreen-rawdata-using-linux
 * https://www.kernel.org/doc/html/latest/input/#
 * 
*/

int main(int argc, char *argv[]) {
    // VARIABLES ---------------------------------------
    static int fd = -1;                                 // TODO: Close fd on SIGINT (Ctrl-C), if it's open
    const string EVENT_DEVICE = "/dev/input/event8";    // Dev file for the touch screen

    struct input_event ev;
    char name[256] = "Unknown";
    fd_set readfds;

    // EVERYTHING ELSE----------------------------------
    // Open device for non-blocking read
    fd = open(EVENT_DEVICE.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd == -1) {
        fprintf(stderr, "%s is not a vaild device\n", EVENT_DEVICE.c_str());
        return EXIT_FAILURE;
    }

    // Print device name
    ioctl(fd, EVIOCGNAME(sizeof(name)), name);
    printf("Reading from:\n");
    printf("  - device file: %s\n", EVENT_DEVICE.c_str());
    printf("  - device name: %s\n", name);

    // Prepare for select(): zero and set the fd into fd_set
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    while (true) {
        const size_t EV_SIZE = sizeof(struct input_event);
        ssize_t size;

        /*
         * For timeout version:
         * select(): timeout-enabled version, stop if no event has
         * occurred until timeout; might be useful e.g. for background
         * tasks.
         * $ struct timeval timeout = { 30, 0 };
         * $ ret = select(fd + 1, &readfds, NULL, NULL, &timeout);
         * 
         * For non-timeout version:
         * select(): no-timeout version, just stop on errors or
         * interrupts.
         * $ ret = select(fd + 1, &readfds, NULL, NULL, NULL);
         */
        int ret = select(fd + 1, &readfds, NULL, NULL, NULL);
        if (ret == -1) {
            perror("Error: select() failed");
            goto err;
        } else if (ret == 0) {
            fprintf(stderr, "Error: select() timeout\n");

        }
        size = read(fd, &ev, EV_SIZE);
        if (size < EV_SIZE) {
            fprintf(stderr, "Error: Wrong size when reading\n");
            goto err;
        }
        
        // Print everything.!!
        cout << 
            "Type: " << ev.type << " "
            "Code: " << ev.code << " "
            "Value: " << ev.value << " "
            "Time: " << ev.time.tv_sec <<
        endl;
    }

    return EXIT_SUCCESS;

err:
    close(fd);
    return EXIT_FAILURE;
}