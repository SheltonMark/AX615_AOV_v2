# Function Description:

This demo is designed to demonstrate the fast boot and sleep-wakeup functionality.
The execution of this demo is configured in /etc/init.d/rcS and is automatically executed during system startup.

The following configurations allow for automatic reboot or sleep-wakeup:

* Automatic Reboot:
  touch /customer/reboot
  After the demo board starts, execute this command to create a reboot file in the customer partition.
  The system will automatically reboot if no person is detected in 5 consecutive frames. Before rebooting, the system will save the first frame image and up to 60 frames of video to the SD card at /tfcard/qsdemo.
* Automatic Sleep-Wakeup (AOV):
  touch /customer/aov
  After the demo board starts, execute the above command in sequence to create the wakeup and aov files in the customer partition.
  If no person is detected, the system will reduce the VIN output frame rate to 1fps, then sleep for 1 second before waking up. This process will repeat until a person is detected, at which point the frame rate will increase to 15fps.
  The video will be saved in a circular manner to the SD card under the /tfcard/qsdemo/ directory. If no SD card is inserted, the video will be saved in the /tmp directory, with about 30 frames of video.

Note: If both the reboot and aov files exist at the same time, the system will execute the fast boot reboot.
