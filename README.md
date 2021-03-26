
# Development Guide:
If you want to build from source, follow these steps, otherwise skip this section.

   1. Install the SoM sdk.sh to the path you choose or default. Suppose SDKPATH.
   2. Run "./build.sh ${SDKPATH}" to build the somapp application.
   3. The build process in 2. will produce a rpm package SoMApp-1.0.1-1.aarch64.rpm under build/, upload to the board, and run "rmp -ivh --force ./SoMApp-1.0.1-1.aarch64.rpm" to update install.


The following file outlines the file structure and instructions on how to use
the smartcam application. This file outlines the HW requirements for the application, how to run the application, and an overview of the application file structure. 

# How to run the example application:

## Hardware and software set-up:

### 1. Monitor:

Before booting, connect the monitor to the board via either DP or HDMI port.

### 2. IAS sensor:  
  
Before power on, install a AR1335 sensor module in J7.

### 3. UART/JTAG interface:

For interacting and seeing boot time information connect a USB debugger to the J4.

### 4. You may also use a USB webcam as an input device. 

The webcam is optional video input device supported in the application.

Recommended is the Logitech BRIO.

### 5. Network connection:

Connect the Ethernet cable to your local network with DHCP enabled or a direct PC connection with a static IP configuration.

### 6. RTSP client:

You will use a PC having network access to the board as the RTSP client machine.

On the client machine, to receive and play the RTSP stream, you need to install ffplay which is part of package FFmpeg.

For Linux, you can install FFmpeg with the package manager of your distribution.

For Windows, you can find install instructions on https://ffmpeg.org/download.html

You may experience a lag of <1s, which maybe bigger / smaller if you use other RTSP clients.

# Firmware

  The accelerated application (AA) firmware consiting of bitstream, device tree overlay (dtbo), and xclbin file are loaded dynamically on user request once Linux is fully booted. The xmutil utility can be used for that purpose.

  * To list the available AA applications, run:

    `xmutil listapps`

    You should see similar output to this:

            Accelerator,     Type,   Active
            kv260-aa2,       flat,   0
            kv260-aa1,       flat,   0

    The Active column shows which AA is currently loaded in the system. It will change to 1 after the firmware is loaded.

  * To load the AA1 application firmware consisting of PL bitstream, device tree overlay and xclbin, run the following command:

    `xmutil loadapp kv260-aa1`

  * After you are done running the application, you can unload the curently loaded AA application firmware by running:

    `xmutil unloadapp`

# Two methods of interacting with the application are provided.

  * A Juypter notebook. Use a web-browser (e.g. Chrome, Firefox) to interact with the platform.

    * The Jupyter notebook URL will be printed to the UART if the board is connected and allocated an IP address at boot. 

                Example: http://<board_ip_addr>:8888
    * If using a direct connection (no DHCP) see public documentation on how to configure your PC with a static IP on the same subnet. 

      For the SOM target set the desired IP address within the same subnet using ifconfig.

      Example: ifconfig eth0 10.0.1.15 netmask 255.255.255.0

      The notebook will be available at http://<your defined IP addr>:8888
       
  * Command line scripts and configurations.
  
    These allow for the user to define different video input and output device targets using the "smartcam" application. These are to be executed using the UART/debug interface. Example scripts and configuration definitions are provided below.

* Example scripts are provided on the system. For location see "File Structure"

  * MIPI RTSP server:

    1. Invoking "01.mipi-rtsp.sh" will start rtsp server for mipi captured images

    2. Script accepts ${width} ${height} as the 1st and 2nd parameter, the default is 1920 x 1080

    3. Run "ffplay rtsp://boardip:5000/test" on the client PC to receive the rtsp stream.- if using a direct connection (no DHCP), see B)a)ii) above to configure a boardip before this step.

    4. Checking:

        You should be able to see the images the camera is capturing on the ffplay window, and when there's face captured by camera, there should be blue box drawn around the face, and the box should follow the movement of the face.

  * MIPI DP display
    1. Make sure the monitor is connected as 0)-a).

    2. Invoking "02.mipi-dp.sh" will play the captured video with detection results on monitor.

    3. Script accepts ${width} ${height} as the 1st and 2nd parameter, the default is 1920 x 1080

    4. Checking:

       You should be able to see the images the camera is capturing on the monitor connected to the board,  and when there's
       face captured by camera, there should be blue box drawn around the face, and the box should follow the movement of the face.

   * File to File
      1. Invoking "03.file-to-file.sh"

      2. Read in the sample video file from "/usr/share/somapp/movies/AA1/walking-people.nv12.30fps.1080p.h264", perform detection and generate video with detection bbox, save as ./out.h264

      3. Checking:

          Play the input video file "/usr/share/somapp/movies/AA1/walking-people.nv12.30fps.1080p.h264" and generated video file "./out.h264" with any media player you prefer, e.g. VLC, ffplay.  
          You should be able to see in the output video file, there are blue boxes around the faces of walking people, and the boxes should follow the movement of the faces, while there's no such boxes with the input video file.
            
            
    * File to DP

      1. Invoking "04.file-ssd-dp.sh"

      2. Read in the sample video file from "/usr/share/somapp/movies/AA1/Road-Adas.nv12.30fps.1080p.h264", perform detection and generate video with detection bbox, and display onto moniter

      3. Checking:

          You should be able to see a video of highway driving, with detection of vehicles in a bounding box.   

# Additional configuration options for smartcam invocation:
   The example scripts and Jupyter notebook work as examples to show capability of the smartcam for specific configurations.
   More combinations could be made based on the options provided by smartcam.
   You can get detailed application options as following by invoking ./smartcam --help.

    Usage:
      smartcam [OPTION?] - Application for face detection on SOM board of Xilinx.
    
    Help Options:
      -h, --help                        Show help options
      --help-all                        Show all help options
      --help-gst                        Show GStreamer Options

    Application Options:
      -m, --mipi                        auto detect MIPI device
      -u, --usb=media_ID                usb camera media device id, e.g. 0 for /dev/media0
      -f, --file=file path              location of h26x file as input
      -i, --infile-type=h264            input file type: [h264 | h265]
      -W, --width=1920                  resolution w of the input
      -H, --height=1080                 resolution h of the input
      -r, --framerate=30                framerate of the input
      -t, --target=dp                   [dp|rtsp|file]
      -o, --outmedia-type=h264          output file type: [h264 | h265]
      -p, --port=5000                   Port to listen on (default: 5000)
      -a, --aitask                      select AI task to be run: [facedetect|ssd|refinedet]
      -n, --nodet                       no AI inference
      -R, --report                      report fps
      --ROI-off                         turn off ROI


   Example of supported combinations sorted by input are outlined below. If using command line to invoke the smartcam stop the process via CTLR-C prior to starting the next instance.

   Note some of the configurations have functional limitations in EA1.
   have limitations.

   * input MIPI (IAS sensor input):
      * output: RTSP

        `smartcam  --mipi -W 1920 -H 1080 --target rtsp >/dev/null 2>&1`

      * output: DP

        `smartcam  --mipi -W 1920 -H 1080 --target dp >/dev/null 2>&1`

      * output: file

        `smartcam  --mipi -W 1920 -H 1080 --target file >/dev/null 2>&1`

   * input file (file on file system):
      * output: RTSP [!Not functional in EA1]

        `smartcam  --file ./test.h264 -i h264 -W 1920 -H 1080 -r 30 --target rtsp >/dev/null 2>&1 `
      * output: DP [!Not functional in EA1]

        `smartcam  --file ./test.h264 -i h264 -W 1920 -H 1080 -r 30 --target dp >/dev/null 2>&1`
      * output: file

        `smartcam  --file ./test.h264 -i h264 -W 1920 -H 1080 -r 30 --target file >/dev/null 2>&1`

      **Note** you must update the command to the specific file desired as input source.

   * input USB (USB webcam):

      * output: RTSP

        `smartcam  --usb 1 -W 1920 -H 1080 -r 30 --target rtsp >/dev/null 2>&1`

      * output: DP

        `smartcam  --usb 1 -W 1920 -H 1080 -r 30 --target dp >/dev/null 2>&1`

      * output: file

        `smartcam  --usb 1 -W 1920 -H 1080 -r 30 --target file >/dev/null 2>&1`
      
      **You must ensure the width/height/framerate defined are supported by your USB camera.**


# File structure:

The application is installed as:

* Binary File: => /opt/xilinx/bin

  smartcam         main app

* Script File: => /opt/xilinx/bin/

  |||
  |-|-|
  |setupmipi.sh     |    find mipi media number, to be used by smartcam.
  |01.mipi-rtsp.sh  |    call smartcam to run facedetction and send out rtsp stream.
  |02.mipi-dp.sh    |    call smartcam to run facedetction and display on DP display.
  |03.file-file.sh  |    call smartcam to run facedetction and display on input h264/5 file and generate output h264/5 with detection boxes.

* configuration File: => /opt/xilinx/share/ivas

  ||
  |-|
  |  kernel_boundingbox_facedetect.json|
  |  kernel_densebox_640_360.json|
  |  kernel_xpp_pipeline.json|

* Jupyter notebook file:  => /usr/share/notebooks/smartcam

  |||
  |-|-|
  |smartcam.ipynb    |        Jupyter notebook file for MIPI->RTSP demo. |

<p align="center"><sup>Copyright&copy; 2021 Xilinx</sup></p>