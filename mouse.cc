#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
using std::string;

int main(int argc, char** argv) {
    int fd, bytes;
    unsigned char data[3];

    const string pDevice = "/dev/input/mice";

    // Open Mouse
    fd = open(pDevice.c_str(), O_RDONLY);
    if(fd == -1) {
        printf("ERROR Opening %s\n", pDevice.c_str());
        return -1;
    }

    signed char x, y;
    int left, middle, right;
    while(1)
    {
        // Read Mouse     
        bytes = read(fd, data, sizeof(data));

        if(bytes > 0) {
            left = data[0] & 0x1;
            right = data[0] & 0x2;
            middle = data[0] & 0x4;

            x = data[1];
            y = data[2];
            printf("x=%d, y=%d, left=%d, middle=%d, right=%d\n", x, y, left, middle, right);
        }   
    }
    return 0; 
}