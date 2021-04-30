## Introduction

Hello everyone. In this course you will be given a set of programming assignments. 
For those of you who are going to complete the course, it is required to implement
most of the labs. These assignments should be implemented on your own machines and then
submitted to the Coursera system for further validation and testing. In this video we are
going to setup your Mac workstation, so that you will be able to start writing programs
that leverage power of OpenCV.

Before we start, lets understand the plan on the video. Firstly, we will take a look at
required pre-requisites. Then, we will examine several ways of installing opencv on your
machine: using pre-configured package for Python, using pre-configured package for Python
and C++, and building opencv from sources. Usually, it is enough to install pre-configured
version, however as you are going to master computer vision, we recommended to build
opencv from sources. So, you need to choose either way and follow corresponding instructions.
Skip other steps and feel free to rewind the video.

## Pre-requisites

First of all, check your OS version. I will assume that you use macOS Catalina or Big Sur. For
example, my machine has 10.15.5. If your macOS
version differ and you see any inconsistencies whith what I am showing, feel free to check
out the forum and ask a question there or just google the problem.

Secondly, we will use Xcode toolset. I will not cover Xcode
installation details in this video, however, for your information, you can easily grab
the Xcode from macOS App Store for free. Make sure you have Apple ID, or create an account
by going to developer.apple.com.

Let us make a short check that Xcode is working properly. For that, open the
Xcode and create a HelloWorld project. 

1. Select a New Xcode Project
1. In the template popup select macOS -> Command Line Tool
1. Name it 'CheckXcode' 
1. Specify organization name: `opencv2d`
1. Select `C++` language
1. Select folder where to keep our project. I use the `opencv2d` directory in the user folder
1. Run by using the play button at the top

Once you see hello world in your console, that means Xcode is properly installed. Press
stop and close the current project.

## Pre-configured OpenCV for Python from PyPi

Nice, now we can start installing OpenCV. The first and the easiest way is to install pre-configured
opencv package for Python. For that we:

1. open the terminal
1. check Python3 installation: `python3 --version`
1. go to official website to learn details: https://pypi.org/project/opencv-python/
1. choose the most complete package and run: `pip install opencv-contrib-python` 
1. check installation by entering the Python REPL: `python3`
1. import tha library: `import cv2`

It works and you can proceed experimenting with it. However, I would remind that we want to
develop C++ application on top of opencv as well. So, we might want to install pre-configured 
opencv package for C++. 


## Pre-configured OpenCV for Python and C++ from homebrew

For that we need to firstly install the Homebrew - 
The Missing Package Manager for macOS. For that, open the website of homebrew. Copy the
recommended install instructions and execute it in the terminal. To check that it is installed
correctly, run `brew help` in the new terminal.

Our next step is installing the opencv itself. For that, open the terminal and type
`brew install opencv`. It will install the latest stable version of opencv. We can check that
opencv is installed by trying to install it again and getting warning that is already installed.

So, we have installed pre-configured opencv package, containing necessary API for C++ development.
However, this package does not contain full list of packages that you will need during the course.

## Build OpenCV for Python and C++ from sources

To install all the packages, you need to build main opencv package and the opencv-contrib package
that contains numerous extensions. To build from sources, we need to perform a set of following
steps:

1. We will use the following link: https://www.pyimagesearch.com/2018/08/17/install-opencv-4-on-macos/
   as the basis for our instructions
1. Let's open the most recent release of opencv to the date of this video capturing:
   https://github.com/opencv/opencv/releases/tag/4.5.1
1. Create a tmp folder for all archives:
   `mkdir ~/opencv4.5-tmp && cd ~/opencv4.5-tmp`
1. We need to download opencv sources:
   `wget https://github.com/opencv/opencv/archive/4.5.1.zip -O opencv.zip`
1. We need to download opencv-contrib sources:
   `wget https://github.com/opencv/opencv_contrib/archive/4.5.1.zip -O opencv_contrib.zip`
1. Unzip the opencv files:
   `unzip opencv.zip`
1. Unzip the opencv-contrib files:
   `unzip opencv_contrib.zip`
1. Move the files to simple directories:
   `mv opencv-4.5.1/ opencv`
1. Move opencv-contrib files to simple directories:
   `mv opencv_contrib-4.5.1/ opencv_contrib`
1. Make build directory:
   `cd opencv && mkdir build && cd build`
1. Copy and run the following command. Install cmake if it is not available on the system.
   ```bash
   cmake -D CMAKE_BUILD_TYPE=DEBUG \
         -D CMAKE_INSTALL_PREFIX=~/opencv4.5-custom \
         -D OPENCV_EXTRA_MODULES_PATH=~/opencv4.5-tmp/opencv_contrib/modules \
         -D BUILD_EXAMPLES=ON ..
   ```
1. Make the project:
   ```
   make -j4
   ```
1. Install opencv:
   ```
   sudo make install
   ```

## Configure Xcode project to work with opencv

Now we can start configuring the fresh Xcode project to work with installed opencv.

1. Open Xcode
1. Create a new macOS project for Commandline
1. We will call it "HelloOpenCV"
1. Specify organization name: `opencv2d`
1. Select `C++` language
1. Select folder where to keep our project. I use the `opencv2d` directory in the user folder
1. Paste the `main.cpp` code to the `main.cpp`

As we can see there is unknown include statement of the opencv package and many
other errors. We need to fix that.

1. Firstly, we need to add all headers of the opencv that we might need during
   the development phase
1. For that we need to select the project, go to 'Build settings' and then to
   `Header Search Paths`
1. Here in the `Debug` section we need to add path to the include directory of
   opencv installation, in particular `/usr/local/Cellar/opencv/4.5.1_2/include`.
   Do not forget to mark the path as recursive.
1. Secondly, we need to add library path of the opencv
1. For that we need to select the project, go to 'Build settings' and then to
   `Library Search Paths`
1. Here in the `Debug` section we need to add path to the lib directory of
   opencv installation, in particular `/usr/local/Cellar/opencv/4.5.1_2/lib`.
   Do not forget to mark the path as recursive.
1. Finally, we need to make sure that Linker can find necessary symbols. For that
   we need to provide all the necessary flags - all libraies that we use in our application.
   In our application it is:
   ```
   -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_core
   ```

   To name the few more available libraries that can be included with the following linker flags:
   ```
   -lopencv_gapi -lopencv_stitching -lopencv_alphamat -lopencv_aruco -lopencv_bgsegm -lopencv_bioinspired -lopencv_ccalib -lopencv_dnn_objdetect -lopencv_dnn_superres -lopencv_dpm -lopencv_face -lopencv_freetype -lopencv_fuzzy -lopencv_hfs -lopencv_img_hash -lopencv_intensity_transform -lopencv_line_descriptor -lopencv_mcc -lopencv_quality -lopencv_rapid -lopencv_reg -lopencv_rgbd -lopencv_saliency -lopencv_sfm -lopencv_stereo -lopencv_structured_light -lopencv_phase_unwrapping -lopencv_superres -lopencv_optflow -lopencv_surface_matching -lopencv_tracking -lopencv_highgui -lopencv_datasets -lopencv_text -lopencv_plot -lopencv_videostab -lopencv_videoio -lopencv_viz -lopencv_xfeatures2d -lopencv_shape -lopencv_ml -lopencv_ximgproc -lopencv_video -lopencv_dnn -lopencv_xobjdetect -lopencv_objdetect -lopencv_calib3d -lopencv_imgcodecs -lopencv_features2d -lopencv_flann -lopencv_xphoto -lopencv_photo -lopencv_imgproc -lopencv_core
   ``` 

## Run opencv sample with image

Now we can see that our include statements were resolved. Before building and running our 
application, ensure that you have placed a test image to the `img` folder of your project.
Feel free to put your selfie here as I have done. So we have put an image to the `img` folder.

Xcode uses the absolute path of the image or resource. In order to set relative paths,
you need to set the Working Directory. To do that, in `Edit Scheme` go to the `Options` tab,
select `Run` in the list and check `Use Custom Working Directory`. Then add the project directory name.

Now we can run the application by clicking Run button or pressing CMD+R.

It works! You are great!

## Run opencv sample with web camera

However, we have more inspiring sample that is showing our web camera output. Comment the
first line and uncomment the second one. Run the application by clicking Run button
or pressing CMD+R. We see the error. In order for Xcode to work with the web camera we
have to allow that by filling the special file `Info.plist` that manages permissions.

Copy the `NSCameraUsageDescription` key and start creating the file. 
File->New->File->Property list. Name it `Info.plist`. Add a key `NSCameraUsageDescription`
with an explanation on why you
need access to camera. For example, `Hello OpenCV needs access to Web Camera`.

Copy this file to the place where your binary lives, by clicking Reveal in Finder in the
context menu. After that run the application again!

We can see that our application is working, congratulations!

## Conclusions

To sum up, we have learned how to configure the macOS machine to work with opencv library,
how to configure a project in Xcode so that you can use opencv library from C++ code.
Each time you need to create a new application, make sure you have properly configured its
paths for compiler and linker.

See you in next episodes!


## LINKS

1. Xcode: https://apps.apple.com/us/app/xcode/id497799835?mt=12
1. OpenCV release: https://github.com/opencv/opencv/releases/tag/4.5.1
1. OpenCV Contrib release: https://github.com/opencv/opencv_contrib/releases/tag/4.5.1
1. opencv-python: https://pypi.org/project/opencv-python/
1. Homebrew: https://brew.sh/
1. Build opencv: https://www.pyimagesearch.com/2018/08/17/install-opencv-4-on-macos/
1. Official guide: https://docs.opencv.org/master/d0/db2/tutorial_macos_install.html
1. My github account: https://github.com/demid5111
1. My avatar: https://avatars.githubusercontent.com/u/4301327?s=460&u=0ac2e48a9e01388caaf51affc2b358e4351524e6&v=4
1. My instructions: https://gist.github.com/demid5111/b5d3a93bdb05554312b161bdedba497e
