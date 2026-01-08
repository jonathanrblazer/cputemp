#include <cstdio>      // printf, fflush
#include <cstdlib>     // atoi
#include <fcntl.h>     // open
#include <unistd.h>    // read, close, sleep

int main()
{
    const char* tempPath = "/sys/class/thermal/thermal_zone0/temp";

    while (true) {
        int fd = open(tempPath, O_RDONLY);
        if (fd < 0) {
            printf("Failed to open CPU temperature file\n");
            fflush(stdout);
            sleep(3);
            continue;
        }

        char buffer[16] = {0};
        int bytesRead = read(fd, buffer, sizeof(buffer) - 1);
        close(fd);

        if (bytesRead > 0) {
            int tempMilliC = atoi(buffer);
            float tempC = tempMilliC / 1000.0f;

            printf("CPU Temperature: %.2f °C\n", tempC);
        } else {
            printf("Failed to read CPU temperature\n");
        }

        fflush(stdout);
        sleep(3);
    }

    return 0;
}
