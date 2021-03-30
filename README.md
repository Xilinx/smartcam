
# Development Guide:

If you want to cross compile the source in Linux PC machine, follow these steps, otherwise skip this section.

1. Refer to the `K260 SOM Starter Kit Tutorial` to build the cross-compilation SDK, and install it to the path you choose or default. Suppose it's SDKPATH.

2. Run "./build.sh ${SDKPATH}" in the source code folder of current application, to build the application. <a name="build-app"></a>

3. The build process in [2](#build-app). will produce a rpm package smartcam-1.0.1-1.aarch64.rpm under build/, upload to the board, and run "rmp -ivh --force ./smartcam-1.0.1-1.aarch64.rpm" to update install.


# Setting up the Board

1. Get the SD Card Image from [[TOOD]] [Boot Image Site](http://xilinx.com/) and follow the instructions in UG1089 to burn the SD card.

2. Hardware Setup:

    * Monitor:
    
      Before booting, connect the monitor to the board via either DP or HDMI port.
    * IAS sensor:
    
      Before power on, install an AR1335 sensor module in J7.
    
    * UART/JTAG interface:
    
      For interacting and seeing boot-time information, connect a USB debugger to the J4.
    
    * You may also use a USB webcam as an input device.
    
      The webcam is optional video input device supported in the application.
         Recommended is the Logitech BRIO.
    
    * Network connection:
    
      Connect the Ethernet cable to your local network with DHCP enabled or a direct PC connection with a static IP configuration.
    
3. Software Preparation:

    You will use a PC having network access to the board as the RTSP client machine.

    On the client machine, to receive and play the RTSP stream, you need to install FFPlay which is part of package FFmpeg.

    For Linux, you can install FFmpeg with the package manager of your distribution.

    For Windows, you can find install instructions on https://ffmpeg.org/download.html

4.  Get the latest application package.

    1.  Get the list of available packages in the feed.

        `xmutil      getpkgs`

    2.  Install the package with dnf install:

        `sudo dnf install packagegroup-kv260-smartcam.noarch`

5.  Dynamically load the application package.

    The firmware consist of bitstream, device tree overlay (dtbo) and xclbin file. The firmware is loaded dynamically on user request once Linux is fully booted. The xmutil utility can be used for that purpose.

    1. Show the list and status of available acceleration platforms and AI Applications:

        `xmutil      listapps`

    2.  Switch to a different platform for different AI Application:

        *  When there's no active accelerator by inspecting with xmutil listapps, just active the one you want to switch.

            `xmutil      loadapp kv260-smartcam`

        *  When there's already an accelerator being activated, unload it first, then switch to the one you want.

            `xmutil      unloadapp `

            `xmutil      loadapp kv260-smartcam`

6. Getting demo video files suitable for the application:

  To be able to demostrate the function of the application in case you have no MIPI and USB camera in hand, we support the file video source too.
  
  You can download video files from the following links, which is of MP4 format, you can transcode it to what we required with following command.
  
  >      ffmpeg -i input-video.mp4 -c:v libx264 -pix_fmt nv12 -r 30 output.nv12.h264

  Demo video:
  
  * Facedet / RefineDet AI Task:
    * https://pixabay.com/videos/alley-people-walk-street-ukraine-39837/
  * ADAS SSD AI Task:
    * https://pixabay.com/videos/freeway-traffic-cars-rainy-truck-8358/


# Run the Application

There are two ways to interact with the application. 

## Juypter notebook.

  Use a web-browser (e.g. Chrome, Firefox) to interact with the platform.

  The Jupyter notebook URL can be find with command:

> sudo jupyter notebook list

    Output example:

> Currently running servers:
>
> `http://ip:port/?token=xxxxxxxxxxxxxxxxxx`  :: /opt/xilinx/share/notebooks

## Command line.
These allow the user to define different video input and output device targets using the "smartcam" application. These are to be executed using the UART/debug interface.

**Notice** The application need to be ran with ***sudo*** .

### Example scripts and configuration definitions are provided below. Example scripts are provided on the system. For location see "File Structure". 


* MIPI RTSP server:

    1. Invoking `"sudo 01.mipi-rtsp.sh"` will start rtsp server for mipi captured images.

    2. Script accepts ${width} ${height} as the 1st and 2nd parameter, the default is 1920 x 1080.

    3. Run "ffplay rtsp://boardip:5000/test" on the client PC to receive the rtsp stream.

    4. if using a direct connection (no DHCP), see B)a)ii) above to configure a boardip before this step.

    5. Checking:

        You should be able to see the images the camera is capturing on the ffplay window, and when there's face captured by camera, there should be blue box drawn around the face, and the box should follow the movement of the face.

* MIPI DP display:

    1. Make sure the monitor is connected as [here](#Setting-up-the-Board).

    2. Invoking `"sudo 02.mipi-dp.sh"` will play the captured video with detection results on monitor.

    3. Script accepts ${width} ${height} as the 1st and 2nd parameter, the default is 1920 x 1080.

    4. Checking:

        You should be able to see the images the camera is capturing on the monitor connected to the board, and when there's face captured by the camera, there should be blue box drawn around the face, and the box should follow the movement of the face.

* File to File

    1. Invoking `"sudo 03.file-to-file.sh"`

        Take the first argument passed to this script as the path to the video file (you can use the demo video for face detection, or similar videos), perform face detection and generate video with detection bbox, save as `./out.h264`
      
    2. Checking:
    
        Play the input video file and generated video file "./out.h264" with any media player you prefer, e.g. VLC, FFPlay. You should be able to see in the output video file, there are blue boxes around the faces of people, and the boxes should follow the movement of the faces, while there're no such boxes with the input video file.

* File to DP

    1. Invoking `"sudo 04.file-ssd-dp.sh"`

        Take the first argument passed to this script as the path to the video file (you can use the demo video for ADAS SSD, or similar videos), perform vehicles detection and generate video with detection bbox, and display onto monitor 
      
    2. Checking: 
    
        You should be able to see a video of highway driving, with the detection of vehicles in a bounding box.

### Additional configuration options for smartcam invocation:

The example scripts and Jupyter notebook work as examples to show the capability of the smartcam for specific configurations. More combinations could be made based on the options provided by smartcam. User can get detailed application options as following by invoking ./smartcam --help.

#### Usage:

```
 smartcam [OPTION?] - Application for face detection on SOM board of Xilinx.

 Help Options:

 -h, --help             Show help options

 --help-all             Show all help options

 --help-gst             Show GStreamer Options

 
 Application Options:

 -m, --mipi=                use MIPI camera as input source, auto detect, fail if no mipi available.

 -u, --usb=media_ID         usb camera video device id, e.g. 2 for /dev/video2

 -f, --file=file            path location of h26x file as input

 -i, --infile-type=h264     input file type: [h264 | h265]

 -W, --width=1920           resolution w of the input

 -H, --height=1080          resolution h of the input

 -r, --framerate=30         framerate of the input

 -t, --target=dp            [dp|rtsp|file]

 -o, --outmedia-type=h264   output file type: [h264 | h265]

 -p, --port=5000            Port to listen on (default: 5000)

 -a, --aitask               select AI task to be run: [facedetect|ssd|refinedet]

 -n, --nodet                no AI inference

 -R, --report               report fps

 --ROI-off                  turn off ROI
```


#### Examples of supported combinations sorted by input are outlined below. 
If using the command line to invoke the smartcam, stop the process via CTLR-C prior to starting the next instance.

**Note** Some of the configurations have functional limitations in EA2.

* MIPI Input (IAS sensor input):

    * output: RTSP

        sudo smartcam --mipi -W 1920 -H 1080 --target rtsp >/dev/null 2>&1

    * output: DP

        sudo smartcam --mipi -W 1920 -H 1080 --target dp >/dev/null 2>&1
output: file

        sudo smartcam --mipi -W 1920 -H 1080 --target file >/dev/null 2>&1

* input file (file on file system):

    * output: RTSP

        sudo smartcam --file ./test.h264 -i h264 -W 1920 -H 1080 -r 30 --target rtsp >/dev/null 2>&1

    * output: DP

        sudo smartcam --file ./test.h264 -i h264 -W 1920 -H 1080 -r 30 --target dp >/dev/null 2>&1

    * output: file

        sudo smartcam --file ./test.h264 -i h264 -W 1920 -H 1080 -r 30 --target file >/dev/null 2>&1

    **Note** You must update the command to the specific file desired as the input source.

* input USB (USB webcam): 

    * output: RTSP

      sudo smartcam --usb 1 -W 1920 -H 1080 -r 30 --target rtsp >/dev/null 2>&1

    * output: DP

      sudo smartcam --usb 1 -W 1920 -H 1080 -r 30 --target dp >/dev/null 2>&1

    * output: file

      sudo smartcam --usb 1 -W 1920 -H 1080 -r 30 --target file >/dev/null 2>&1

    **Note** You must ensure the width/height/framerate defined are supported by your USB camera.

# Files structure of the application

* The application is installed as:

    * Binary File: => /opt/xilinx/bin

      | filename | description |
      |----------|-------------|
      |smartcam| main app|

    * Script File: => /opt/xilinx/bin/

      | filename | description |
      |----------|-------------|
      |`01.mipi-rtsp.sh` | call smartcam to run facedetction and send out rtsp stream.|
      |`02.mipi-dp.sh  ` | call smartcam to run facedetction and display on DP display.|
      |`03.file-file.sh` | call smartcam to run facedetction and display on input h264/5 file and generate output h264/5 with detection boxes.|

    * configuration File: => /opt/xilinx/share/ivas/smartcam/${AITASK}
    AITASK = "facedetect" | "refinedet" | "ssd"

      | filename | description |
      |----------|-------------|
      |preprocess.json|  Config of preprocess for AI inference|
      |aiinference.json| Config of AI inference (facedetect\|refinedet\|ssd) |
      |drawresult.json| Config of boundbox drawing |

    * Jupyter notebook file: => /opt/xilinx/share/notebooks/smartcam

      | filename | description |
      |----------|-------------|
      |smartcam.ipynb | Jupyter notebook file for MIPI --> DP/RTSP demo.|

<p align="center"><sup>Copyright&copy; 2021 Xilinx</sup></p>
