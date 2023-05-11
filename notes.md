Read in terminal using:                 \
$ sudo stdbuf -oL -eL libinput debug-events --device /dev/input/by-path/platform-AMDI0010:01-event

List devices using:                     \
$ less /proc/bus/input/devices

List Input by-path using                \
$ ls -lh /dev/input/by-path             \
$ udevadm info --export-db | awk '/ID_INPUT_TOUCHSCREEN=1/' RS= | grep "^E: DEVPATH="

List accelerometers using:              \
$ udevadm info --export-db | grep -C 7 accel

Test sensors using:                     \
$ ksystemstats                          \
$ gdbus introspect --system --dest net.hadess.SensorProxy --object-path /net/hadess/SensorProxy

Dependencies:                           \
$ evtest                                \
$ libinput


Supported events for evtest:            \
  Event type 0 (EV_SYN)                 \
  Event type 1 (EV_KEY)                 \
    Event code 330 (BTN_TOUCH)          \
  Event type 3 (EV_ABS)                 \
    Event code 0 (ABS_X)                \
      Value    940                      \
      Min        0                      \
      Max     3600                      \
      Resolution      12                \
    Event code 1 (ABS_Y)                \
      Value    606                      \
      Min        0                      \
      Max     2064                      \
      Resolution      12                \
    Event code 47 (ABS_MT_SLOT)         \
      Value      1                      \
      Min        0                      \
      Max        9                      \
    Event code 48 (ABS_MT_TOUCH_MAJOR)  \
      Value      0                      \
      Min        0                      \
      Max      255                      \
    Event code 49 (ABS_MT_TOUCH_MINOR)  \
      Value      0                      \
      Min        0                      \
      Max      255                      \
    Event code 52 (ABS_MT_ORIENTATION)  \
      Value      0                      \
      Min        0                      \
      Max        1                      \
    Event code 53 (ABS_MT_POSITION_X)   \
      Value      0                      \
      Min        0                      \
      Max     3600                      \
      Resolution      12                \
    Event code 54 (ABS_MT_POSITION_Y)   \
      Value      0                      \
      Min        0                      \
      Max     2064                      \
      Resolution      12                \
    Event code 57 (ABS_MT_TRACKING_ID)  \
      Value      0                      \
      Min        0                      \
      Max    65535                      \
    Event code 60 (ABS_MT_TOOL_X)       \
      Value      0                      \
      Min        0                      \
      Max     3600                      \
      Resolution      12                \
    Event code 61 (ABS_MT_TOOL_Y)       \
      Value      0                      \
      Min        0                      \
      Max     2064                      \
      Resolution      12                \
  Event type 4 (EV_MSC)                 \
    Event code 5 (MSC_TIMESTAMP)