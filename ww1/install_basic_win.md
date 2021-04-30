## Introduction

Hello everyone. In this course you will be given a set of programming assignments. 
For those of you who are going to complete the course, it is required to implement
most of the labs. These assignments should be implemented on your own machines and then
submitted to the Coursera system for further validation and testing. In this video we are
going to setup your Windows workstation, so that you will be able to start writing programs
that leverage power of OpenCV.

Before we start, lets understand the plan on the video. Firstly, we will take a look at
required pre-requisites. Then, we will examine several ways of installing opencv on your
machine: using pre-configured package for Python, using pre-configured package for Python
and C++, and building opencv from sources. Usually, it is enough to install pre-configured
version, however as you are going to master computer vision, we recommended to build
opencv from sources. So, you need to choose either way and follow corresponding instructions.
Skip other steps and feel free to rewind the video. 

## Pre-requisites

First of all, check your OS version. I will assume that you use Windows 10. If your Windows
version differ and you see any inconsistencies whith what I am showing, feel free to check
out the forum and ask a question there.

Secondly, we will use Visual Studio as an IDE of choice. I will not cover Visual Studio
installation details in this video, however, for your information, you can easily grab
the Visual Studio installer.  Luckily, there is a community edition which is free. So, you
just need to download the installer and proceed with the suggested instructions. Make
sure that you have selected the option 'Desktop development Kit' in the installer, otherwise
you need to modify the installation and install these packages before going further.

Let us make a short check that Visual Studio is working properly. For that, open the
Visual Studio and create a HelloWorld project. 

1. Select the New Empty Project
1. Name it 'CheckVisualStudio' and place it where we will place our sample projects:
   `C:\projects\opencv2d\`
1. Create file `main.cpp`
1. Add following content:
```cpp
#include <iostream>

int main() {
	std::cout << "Hello, World!" << std::endl;
}
```
1. Set the target to x64
1. Run Run -> Start without debugging - it will build the solution and execute your application
Once you see hello world in your console, that means Visual Studio is properly installed. Press
any key to close the terminal and close the current project.

## Pre-configured OpenCV for Python from PyPi

Nice, now we can start installing OpenCV. The first and the easiest way is to install pre-configured
opencv package for Python. I assume that you have python3 installed. If not, you can
follow the official install guide. For that we:

1. open the terminal (Ctrl+R + cmd)
1. check Python3 installation: `py --version`
1. go to official website to learn details: https://pypi.org/project/opencv-python/
1. choose the most complete package and run: `py -m pip install opencv-contrib-python` 
1. check installation by entering the Python REPL: `py`
1. import tha library: `import cv2`

It works and you can proceed experimenting with it. However, I would remind that we want to
develop C++ application on top of opencv as well. So, we might want to install pre-configured 
opencv package for C++. 

## Pre-configured OpenCV for Python and C++

Nice, now we can start installing OpenCV. For that we need to proceed to the product
page and find the most recent and stable release version. To the moment of capturing
this video this is the 4.5.1 version. We need to download the executable file. Here it
is <DOWNLOAD>. Once it is downloaded, we need to run it.

It will request the folder where to extract all the files. let's use the `C:\` disk root with
a folder `opencv`. So, copy and paste `C:\opencv`.

Once extraction is done, let's open the folder we specified for the installer to ensure all the
needed files are there.

Now we need to tell the system where it can find the opencv installation. In particular, we need
the system do know where is the pre-built opencv library placed. Firstly, we can find it ourselves.

We open the explorer, navigate to the `C:\opencv` and then go deeper to build, selecting our
architecture `x64`, then as we are using Visual Studio 2019, we need to use the vc15 folder as it
stands for the compiler version shipped with the Visual Studio. For example, if we had 
Visual Studio 17 installed we would select vc14. And then in the `bin` folder we have the desired 
library. This is the path that we need to make the system to remember.

It is made by appending
the system enviornment variable. In order to do that we need to open the Windows Control panel,
by typing `env` in Windows search. After a security confirmation, you can see the dialog window,
where you need to click `Environment Variables` button. This is the list of currently 
available environment variables. So, you need to double click on the row with `Path` and then
append the path to opencv to the end of the list. Now we need to restart the system as the best
way to ensure that system and all application recognize environment changes.

Once you are back, check that enviornment variable now has the path to opencv. For that open
Powershell and type `$env:Path`. 

So, we have installed pre-configured opencv package, containing necessary API for C++ development.
However, this package does not contain full list of packages that you will need during the course.

## Build OpenCV for Python and C++ from sources

To install all the packages, you need to build main opencv package and the opencv-contrib package
that contains numerous extensions. There are two pre-requisites: cmake and git bash.

Install CMake using the following instructions. They are straigtforward, however, make sure that you
select "Append to the PATH" for all users or for a current one. After installation completes, you
can check that cmake is installed. For that:

1. open the terminal
1. type `cmake --version`

Nice, now we need to install Git Bash. Installation of Git Bash does not require any additional moves,
simple installation procedure. Once you install it, check by typing Git Bash in Windows search.

As soon as we have CMake and Git Bash we can proceed to building opencv.

To build from sources, we need to perform a set of following
steps:

1. We will use the following link: 
   https://cv-tricks.com/how-to/installation-of-opencv-4-1-0-in-windows-10-from-source/
   as the basis for our instructions
1. Let's open the most recent release of opencv to the date of this video capturing:
   https://github.com/opencv/opencv/releases/tag/4.5.1 and download it.
1. Let's open the most recent release of opencv_contrib to the date of this video capturing:
   https://github.com/opencv/opencv_contrib/releases/tag/4.5.1 and download it.
1. Unzip the opencv files
1. Unzip the opencv-contrib files
1. Run cmake-gui
1. choose the directory with opencv sources
1. specify the directory for storing building binaries and Visual Studio project.
   it would better if you create a new one and specify it here
1. press 'Configure'
1. use standard settings and proceed with Finish button
1. once project is prepared, review all the flags suggested
1. I suggest selecting `BUILD_WITH_DEBUG_INFO`
1. however, what we should focus on is `OPENCV_EXTRA_MODULES_PATH`. We need to specify path to 
   contrib folder we extracted earlier: 
   `C:/Users/demidovs/Downloads/WW1-INSTALL-OPENCV/opencv_contrib-4.5.1/modules`
1. also, make sure you will install opencv in the proper directory. Let me change `CMAKE_INSTALL_PREFIX`
   to `C:/opencv-custom`
1. when you are ready, press `Generate`.
1. once generation is done, let's inspect the build directory: 
   `C:\Users\demidovs\Downloads\WW1-INSTALL-OPENCV\opencv-4.5.1\build`. It contains the Visual Studio project
   that we will use for installation of opencv
1. double click on `OpenCV.sln`
1. build `BUILD_ALL` by right click on the corresponding project
1. Then build `INSTALL` by right click on the corresponding project to finally install
1. Let's inspect our installation directory
1. This is the path that we need to make the system to remember.

   It is made by appending
   the system enviornment variable. In order to do that we need to open the Windows Control panel,
   by typing `env` in Windows search. After a security confirmation, you can see the dialog window,
   where you need to click `Environment Variables` button. This is the list of currently 
   available environment variables. So, you need to double click on the row with `Path` and then
   append the path to opencv to the end of the list. Now we need to restart the system as the best
   way to ensure that system and all application recognize environment changes.

   Once you are back, check that enviornment variable now has the path to opencv. For that open
   Powershell and type `$env:Path`. 

## Configure Visual Studio project to work with opencv

Nice, we have it and can proceed to making our first sample
with OpenCV!

Go to the Visual Studio and create the new empty C++ project. 

1. We will call it "HelloOpenCV".
1. Set the target to `Debug` and `x64`. This is very important to configure it for x64.
1. Paste the `main.cpp` code to the `main.cpp`

As we can see there is unknown include statement of the opencv package. We need to fix that.

1. For that we need to configure a project so that compiler could know where to take headers
and library from. Open the Project->HelloOpenCV Properties and go to `VC++ Directories`.
1. Change the `Include Directories` to contain corresponding include folder in the opencv 
installation. Insert it to the beginning of the value: `C:\opencv\opencv\build\include;`
1. Change the `Library directories` to include `C:\opencv\opencv\build\x64\vc15\lib`
1. Change the `Linker->Input->Additional dependencies` to
   1. include `opencv_world451d.lib` if we are talking about pre-configured package installation
   1. include `opencv_core451d.lib;opencv_highgui451d.lib;opencv_videoio451d.lib;opencv_imgcodecs451d.lib;`
      if we are talking about building from sources. In future, this list might be bigger 
      containing all the libraries and extensions that you use from opencv and opencv-contrib.
1. Apply and close the window

## Run opencv sample with image

Now we can see that our include statements were resolved. Before building and running our 
application, ensure that you have placed a test image to the `img` folder of your project.
Feel free to put your selfie here as I have done. So we have put an image to the `img` folder.
Open the image first, it might request allowing viewing untrusted resources.

Now we can run the application by clicking Debug->Start without debugging.

We can see that our application is working, congratulations!

## Run opencv sample with web camera

However, we have more inspiring sample that is showing our web camera output. Comment the
first line and uncomment the second one. Run the application by clicking Run button
or pressing CMD+R.

It works! You are great!

To sum up, we have learned how to configure the Windows machine to work with opencv library,
how to configure a project in Visual Studio so that you can use opencv library from C++ code.
Each time you need to create a new application, make sure you have properly configured its
paths for compiler and linker.

See you in next episodes!


LINKS:
1. Visual Studio Community: https://visualstudio.microsoft.com/
1. Python installation official guide: https://docs.python.org/3/using/windows.html
1. Python download page: https://www.python.org/downloads/windows/
1. OpenCV installation guide: https://docs.opencv.org/master/d3/d52/tutorial_windows_install.html
1. Build OpenCV guide: https://cv-tricks.com/how-to/installation-of-opencv-4-1-0-in-windows-10-from-source/
1. CMake installation guide: https://cmake.org/install/
1. CMake download page: https://cmake.org/download/
1. Git Bash install page: https://gitforwindows.org/
1. OpenCV release: https://github.com/opencv/opencv/releases/tag/4.5.1
1. OpenCV Contrib release: https://github.com/opencv/opencv_contrib/releases/tag/4.5.1
1. My github account: https://github.com/demid5111
1. My avatar: https://avatars.githubusercontent.com/u/4301327?s=460&u=0ac2e48a9e01388caaf51affc2b358e4351524e6&v=4
1. My instructions: https://gist.github.com/demid5111/6faa590e4fc5813550dd95cc1c538893
