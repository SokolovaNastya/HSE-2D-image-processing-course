## Introduction

Hello everyone. In this course you will be given a set of programming assignments. 
For those of you who are going to complete the course, it is required to implement
most of the labs. These assignments should be implemented on your own machines and then
submitted to the Coursera system for further validation and testing. In this video we are
going to setup your Ubuntu 18.04 workstation, so that you will be able to start writing programs
that leverage power of OpenCV.

Before we start, lets understand the plan on the video. Firstly, we will take a look at
required pre-requisites. Then, we will examine several ways of installing opencv on your
machine: using pre-configured package for Python, using pre-configured package for Python
and C++, and building opencv from sources. Usually, it is enough to install pre-configured
version, however as you are going to master computer vision, we recommended to build
opencv from sources. So, you need to choose either way and follow corresponding instructions.
Skip other steps and feel free to rewind the video.

## Pre-requisites

First of all, check your OS version. I will assume that you use Ubuntu 18.04. If your Ubuntu
version differ or you have a different Linux distribution installed and you see any 
inconsistencies whith what I am showing, feel free to check
out the forum and ask a question there or just google the problem.

## Pre-configured OpenCV for Python from PyPi

Nice, now we can start installing OpenCV. The first and the easiest way is to install pre-configured
opencv package for Python. For that we:

1. open the terminal
1. check Python3 installation: `python3 --version`
1. go to official website to learn details: https://pypi.org/project/opencv-python/
1. choose the most complete package and run: `python3 -m pip install opencv-contrib-python`
   often you will not find the pip installed by default, therefore use the steps from the 
   following instructions to install it 
   (https://www.pyimagesearch.com/2018/05/28/ubuntu-18-04-how-to-install-opencv/)
1. check installation by entering the Python REPL: `python3`
1. import the library: `import cv2`

There are some other paths to install it from the apt repositories by using the command 
`sudo apt get install python3-opencv` but it can install you the previous major
version of opencv which is opencv3.

It works and you can proceed experimenting with it. However, I would remind that we want to
develop C++ application on top of opencv as well. So, we might want to install pre-configured 
opencv package for C++.

## Build OpenCV for Python and C++ from sources

To install all the packages, you need to build main opencv package and the opencv-contrib package
that contains numerous extensions. To build from sources, we need to perform a set of following
steps:

1. Check pre-requisites: cmake and the compilers gcc, g++ should be installed
1. We will use the following link: https://docs.opencv.org/master/d2/de6/tutorial_py_setup_in_ubuntu.html
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
         -DOPENCV_GENERATE_PKGCONFIG=ON \
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
1. Ensure that it is updated in the library storage:
   ```
   sudo ldconfig
   ```

## Create a sample to work with opencv

Now we can start configuring the fresh Xcode project to work with installed opencv.

1. Open your editor of choice (VSCode in my case)
1. Create a folder "HelloOpenCV"
1. Paste the `main.cpp` code to the `main.cpp`

Now we need to compile our application.

1. Firstly, let's try to compile our application with g++ as usual:
   ```
   g++ -Wall -std=c++11 -o main main.cpp
   ```
   We see that it cannot find our library headers
1. For that we need to provide path to the headers and linker flags. The best way to find them
   all in one place is to use the utility module pkg-config. Remember we provided an additional
   argument to our cmake generation? So, let's execute the following:
   ```
   export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/home/parallels/opencv4.5-custom/lib/pkgconfig
   pkg-config --cflags --libs opencv4
   ``` 
1. Here we see the header flags, let's use them only:
   ```
   g++ -Wall -std=c++11 -o main main.cpp -I/home/parallels/opencv4.5-custom/include/opencv4 
   ```
1.   Now there are linker flags needed:
   ```
   g++ -Wall -std=c++11 -o main main.cpp -I/home/parallels/opencv4.5-custom/include/opencv4 -L/home/parallels/opencv4.5-custom/lib -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_core
   ```
   It is a good practice to redirect full output of the package config app to the compilation process:
   ```
   g++ -Wall -std=c++11 -o main main.cpp $(pkg-config --cflags --libs opencv4)
   ```
   However, it is better to explicitly state whqt your application actually needs.

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
or pressing CMD+R.

We can see that our application is working, congratulations!

## Conclusions

To sum up, we have learned how to configure the Ubuntu 18.04 machine to work with opencv library,
how to compile our C++ application with opencv library.
Each time you need to create a new application, make sure you have properly configured its
paths for compiler and linker.

See you in next episodes!


## LINKS

1. Installing instructions: https://www.pyimagesearch.com/2018/05/28/ubuntu-18-04-how-to-install-opencv/
1. opencv-python: https://pypi.org/project/opencv-python/
1. Official guide: https://docs.opencv.org/master/d2/de6/tutorial_py_setup_in_ubuntu.html
1. My github account: https://github.com/demid5111
1. My avatar: https://avatars.githubusercontent.com/u/4301327?s=460&u=0ac2e48a9e01388caaf51affc2b358e4351524e6&v=4
1. My instructions: https://gist.github.com/demid5111/634a7b1a11b9522ac25f9584a9ef1ef8
