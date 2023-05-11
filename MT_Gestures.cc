#include <linux/input.h>
#include <algorithm>    // std::sort
#include <iostream>     // cout, endl
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>      // open, read, EVIOCGNAME, FD_ZERO, FD_SET
#include <math.h>       // max
#include <string>
#include <deque>
#include <map>
using std::map;
using std::cout;
using std::endl;
using std::deque;
using std::string;
/*
 *
 * https://stackoverflow.com/questions/28841139/how-to-get-coordinates-of-touchscreen-rawdata-using-linux
 * https://www.kernel.org/doc/html/latest/input/#
 * 
*/
struct MT_Gestures {
    char code;
    signed int value;
};

signed int MT_SLOTS = 0;
map<long int, deque<MT_Gestures>> MT_POSITIONS;
void InterpreteAndRefresh() {
    // Do not waste time.!!
    if (MT_POSITIONS.size() == 0) return;
    MT_SLOTS = MT_SLOTS + 1;

    // Change in direction
    int index = 0;
    map<char, deque<long int>> Prior;
    deque< map<char, long int> > queue;
    for (const auto& [time, List]: MT_POSITIONS) {
        // At this time, find the average position of all points
        map<char, deque<long int>> Event;
        for (const auto L: List) Event[L.code].push_back(L.value);
        std::sort(Event['x'].begin(), Event['x'].end());
        std::sort(Event['y'].begin(), Event['y'].end());
        // All or Nothing for the number of slots available
        if (
            index != 0
            && Prior['x'].size() == Event['x'].size()
            && Prior['y'].size() == Event['y'].size()
            && (Prior['x'].size() == 0 || Prior['x'].size() == MT_SLOTS)
            && (Prior['y'].size() == 0 || Prior['y'].size() == MT_SLOTS)
        ) {
            // Find the difference between both timestamps
            int AvgX = 0, AvgY = 0;
            for (int i = 0; i < Prior['x'].size(); i++)
                AvgX = AvgX + (Event['x'][i] - Prior['x'][i]);
            for (int i = 0; i < Event['y'].size(); i++)
                AvgY = AvgY + (Event['y'][i] - Prior['y'][i]);
            // Find the average difference between both timestamps
            AvgX = AvgX / MT_SLOTS, AvgY = AvgY / MT_SLOTS;
            // Conditions to append new(1) -----------------------------------
            if (queue.size() == 0) queue.push_back({{'x', AvgX}, {'y', AvgY}});
            else {
                int lidx = queue.size() - 1;
                string Pk = "", Ek = "";
                Ek += (AvgX == 0)? "0": "1"; 
                Ek += (AvgY == 0)? "0": "1";
                Pk += (queue[lidx]['x'] == 0)? "0": "1"; 
                Pk += (queue[lidx]['y'] == 0)? "0": "1";
                // Conditions to append new(2) -----------------------------------
                if (Pk != Ek) queue.push_back({{'x', AvgX}, {'y', AvgY}});
                // Conditions to update old --------------------------------------
                else {
                    queue[lidx]['x'] += AvgX;
                    queue[lidx]['y'] += AvgY;
                }
            }
        }
        // Increment.!! ----------------------------------------
        Prior = Event;
        index++;
    }   MT_POSITIONS.clear();
    // for (auto c: queue) cout << c.second << c.first << " ";
    cout << endl;
    MT_SLOTS = 0;
}

int main() {
    int NO_INPUT_COUNTER = 0;
    // VARIABLES ---------------------------------------
    static int fd = -1;     // TODO: Close fd on SIGINT (Ctrl-C), if it's open
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
        // Error Checkers: Timeout select(), reading size
        struct timeval timeout = { 0, 30 };
        int ret = select(fd + 1, &readfds, NULL, NULL, &timeout);
        if (ret == -1) {
            perror("Error: select() failed");
            goto err;
        } //else if (ret == 0) {
            //fprintf(stderr, "Error: select() timeout\n");
        //}
        const size_t EV_SIZE = sizeof(struct input_event);
        ssize_t size = read(fd, &ev, EV_SIZE);
        if (size < EV_SIZE) {
            fprintf(stderr, "Error: Wrong size when reading\n");
            goto err;
        }
        // Multitouch gestures: grab a bunch and interprete
        if (ev.type == EV_ABS) {
            if (ev.code == ABS_MT_SLOT) {
                MT_SLOTS = std::max(MT_SLOTS, ev.value);
                // cout << "FingersOnScreen: " << ev.value << endl;
            }
            if (ev.code == ABS_MT_POSITION_X) {
                MT_POSITIONS[ev.time.tv_usec].push_back({
                    .code = 'x',
                    .value = ev.value
                });
                cout << "[" << ev.time.tv_usec << "] MT Position X: " << ev.value << endl;
            }
            if (ev.code == ABS_MT_POSITION_Y) {
                MT_POSITIONS[ev.time.tv_usec].push_back({
                    .code = 'y',
                    .value = ev.value
                });
                cout << "[" << ev.time.tv_usec << "] MT Position Y: " << ev.value << endl;
            }
        }
        // Handle counter and interprete prompt
        if (ev.type == EV_SYN) NO_INPUT_COUNTER++;
        else  NO_INPUT_COUNTER = 0;     // Reset
        if (NO_INPUT_COUNTER >= 1000) InterpreteAndRefresh();
        
    }
    return EXIT_SUCCESS;
err:
    close(fd);
    return EXIT_FAILURE;
}