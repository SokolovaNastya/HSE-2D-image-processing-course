#include <vector>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include <opencv2/tracking/tracking.hpp>

using namespace std;

bool segmentation = false;
bool br = false;
bool tracking = false;
bool face_tracking = false;
bool optical_flow = false;
string img_path = "";
cv::VideoCapture capture;

void printUsage(char** argv)
{
    cout <<
        "Week 6\n" << "Usage:\n" << argv[0] << " [Flags] \n"
        "Flags:\n"
        "  -mode (segmentation|background_remover|optical_flow|tracking|face_tracking)\n"
        "  -img (path to image)\n"
        "  -video (path to video)\n"
        "\n"
        "Examples:\n"
        "-mode segmentation -img cards.png\n"
        "-mode background_remover -video vtest.avi\n"
        "-mode optical_flow -video vtest.avi\n"
        "-mode tracking -video vtest.avi\n"
        "-mode face_tracking\n";
}

int parseCmdArgs(int argc, char** argv)
{
    if (argc == 1)
    {
        printUsage(argv);
        return EXIT_FAILURE;
    }

    bool img_file = false;
    bool video_file = false;

    for (int i = 1; i < argc; ++i)
    {
        if (string(argv[i]) == "--help" || string(argv[i]) == "/?")
        {
            printUsage(argv);
            return EXIT_FAILURE;
        }
        else if (string(argv[i]) == "-mode")
        {
            if (string(argv[i + 1]) == "segmentation")
            {
                segmentation = true;
            }
            else if (string(argv[i + 1]) == "background_remover")
            {
                br = true;
            }
            else if (string(argv[i + 1]) == "optical_flow")
            {
                optical_flow = true;
            }
            else if (string(argv[i + 1]) == "tracking")
            {
                tracking = true;
            }
            else if (string(argv[i + 1]) == "face_tracking")
            {
                face_tracking = true;
            }
            else
            {
                cout << "Bad -mode flag value" << endl;
                return EXIT_FAILURE;
            }

            i++;
        }
        else if (string(argv[i]) == "-img")
        {
            if (!string(argv[i + 1]).empty()) {
                cv::Mat img = cv::imread(cv::samples::findFile(argv[i+1]));
                if (img.empty())
                {
                    cout << "Can't read image '" << argv[i+1] << endl;
                    return EXIT_FAILURE;
                }

                img_path = argv[i + 1];
                img_file = true;
            }
            else
            {
                cout << "Bad -img flag value" << endl;
                return EXIT_FAILURE;
            }
        }
        else if (string(argv[i]) == "-video")
        {
            if (!string(argv[i + 1]).empty()) {
                capture = cv::VideoCapture(argv[i + 1]);
                if (!capture.isOpened()) {
                    cout << "Unable to open video file!" << endl;
                    return EXIT_FAILURE;
                }

                video_file = true;
            }
            else
            {
                cout << "Bad -video flag value" << endl;
                return EXIT_FAILURE;
            }
        }
    }

    if (!(tracking || face_tracking || optical_flow || br || segmentation)) {
        cout << "Please, set -mode" << endl;
        return EXIT_FAILURE;
    }

    if (segmentation && !img_file) {
        cout << "Please, load -img" << endl;
        return EXIT_FAILURE;
    }
    else if ((tracking || optical_flow || br) && !video_file) {
        cout << "Please, load -video" << endl;
        return EXIT_FAILURE;
    }

    if (face_tracking && (img_file || video_file)) {
        cout << "Face tracking is only for Web camera mode" << endl;
        cout << "Please, use the following cmd (without img and video): -mode face_tracking" << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void show(string window_name, cv::Mat img)
{
    cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);
    cv::moveWindow(window_name, 700, 150);
    cv::imshow(window_name, img);
    cv::waitKey(0);
};

inline bool fileExists(const string& name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

void grabCut_method() {
    cv::Mat grabCut_img = cv::imread(img_path);
    cv::Mat mask, bgdModel, fgdModel;

    cv::Rect rect(50, 50, 500, 400);
    cv::grabCut(grabCut_img, mask, rect, bgdModel, fgdModel, 1, cv::GC_INIT_WITH_RECT);

    // Foreground
    // https://docs.opencv.org/3.4/d7/d1b/group__imgproc__misc.html#gad43d3e4208d3cf025d8304156b02ba38
    for (auto i = 0; i < grabCut_img.rows; i++) {
        for (auto j = 0; j < grabCut_img.cols; j++) {
            if (mask.ptr(i, j)[0] == 0 || mask.ptr(i, j)[0] == 2) {
                grabCut_img.ptr(i, j)[0] = 0;
                grabCut_img.ptr(i, j)[1] = 0;
                grabCut_img.ptr(i, j)[2] = 0;
            }
        }
    }

    // Grab rect
    cv::Scalar color(0, 0, 255);
    cv::Point point1(rect.x, rect.y);
    cv::Point point2(rect.x + rect.width, rect.y + rect.height);
    cv::rectangle(grabCut_img, point1, point2, color);
    show("GrabCut Output", grabCut_img);
}

void distTrans_method() {
    cv::Mat distT_img = cv::imread(img_path);
    cv::Mat mask;

    inRange(distT_img, cv::Scalar(255, 255, 255), cv::Scalar(255, 255, 255), mask);
    distT_img.setTo(cv::Scalar(0, 0, 0), mask);
    show("Black Background Image", distT_img);

    cv::Mat kernel = (cv::Mat_<float>(3, 3) <<
        1, 1, 1,
        1, -8, 1,
        1, 1, 1);
    cv::Mat imgLaplacian;
    filter2D(distT_img, imgLaplacian, CV_32F, kernel);

    cv::Mat sharp;
    distT_img.convertTo(sharp, CV_32F);
    cv::Mat imgResult = sharp - imgLaplacian;
    imgResult.convertTo(imgResult, CV_8UC3);
    imgLaplacian.convertTo(imgLaplacian, CV_8UC3);
    show("New Sharped Image", imgResult);

    cv::Mat bw;
    cvtColor(imgResult, bw, cv::COLOR_BGR2GRAY);
    threshold(bw, bw, 40, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    show("Binary Image", bw);

    cv::Mat dist;
    distanceTransform(bw, dist, cv::DIST_L2, 3);
    normalize(dist, dist, 0, 1.0, cv::NORM_MINMAX);
    show("Distance Transform Image", dist);

    threshold(dist, dist, 0.4, 1.0, cv::THRESH_BINARY);

    cv::Mat kernel1 = cv::Mat::ones(3, 3, CV_8U);
    dilate(dist, dist, kernel1);
    show("Peaks", dist);

    cv::Mat dist_8u;
    dist.convertTo(dist_8u, CV_8U);
    vector<vector<cv::Point> > contours;
    findContours(dist_8u, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    cv::Mat markers = cv::Mat::zeros(dist.size(), CV_32S);

    for (size_t i = 0; i < contours.size(); i++)
    {
        drawContours(markers, contours, static_cast<int>(i), cv::Scalar(static_cast<int>(i) + 1), -1);
    }

    circle(markers, cv::Point(5, 5), 3, cv::Scalar(255), -1);
    cv::Mat markers8u;
    markers.convertTo(markers8u, CV_8U, 10);
    show("Markers", markers8u);

    watershed(imgResult, markers);
    cv::Mat mark;
    markers.convertTo(mark, CV_8U);
    bitwise_not(mark, mark);

    vector<cv::Vec3b> colors;
    for (size_t i = 0; i < contours.size(); i++)
    {
        int b = cv::theRNG().uniform(0, 256);
        int g = cv::theRNG().uniform(0, 256);
        int r = cv::theRNG().uniform(0, 256);
        colors.push_back(cv::Vec3b((uchar)b, (uchar)g, (uchar)r));
    }

    cv::Mat dst = cv::Mat::zeros(markers.size(), CV_8UC3);

    for (int i = 0; i < markers.rows; i++)
    {
        for (int j = 0; j < markers.cols; j++)
        {
            int index = markers.at<int>(i, j);
            if (index > 0 && index <= static_cast<int>(contours.size()))
            {
                dst.at<cv::Vec3b>(i, j) = colors[index - 1];
            }
        }
    }

    show("Distance Transform and Watershed Algorithm", dst);
}

void br_method(cv::Ptr<cv::BackgroundSubtractor> pBackSub) {
    cv::Mat frame, fgMask, output;

    while (true) {
        capture >> frame;
        if (frame.empty())
            break;

        pBackSub->apply(frame, fgMask);

        output = cv::Scalar::all(0);
        frame.copyTo(output, fgMask);

        stringstream ss;
        ss << capture.get(cv::CAP_PROP_POS_FRAMES);

        imshow("FG Mask", fgMask);
        imshow("Background remover", output);

        int keyboard = cv::waitKey(30);
        if (keyboard == 'q' || keyboard == 27)
            break;
    }
}

void simple_track(){
    // Types: "BOOSTING", "MIL", "KCF", "TLD","MEDIANFLOW", "GOTURN", "MOSSE", "CSRT" };
    cv::Ptr<cv::Tracker> tracker = cv::TrackerKCF::create();

    cv::Mat frame;
    capture >> frame;
    cv::Rect bbox(260, 200, 80, 100);
    rectangle(frame, bbox, cv::Scalar(255, 0, 0), 2, 1);

    tracker->init(frame, bbox);

    while (true)
    {
        capture >> frame;
        if (frame.empty())
            break;

        if (tracker->update(frame, bbox))
            rectangle(frame, bbox, cv::Scalar(255, 0, 0), 2, 1);
        else
            putText(frame, "Tracking failure detected", cv::Point(100, 80), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(0, 0, 255), 2);

        stringstream ss;
        ss << capture.get(cv::CAP_PROP_POS_FRAMES);
        
        imshow("Tracking", frame);

        int keyboard = cv::waitKey(30);
        if (keyboard == 'q' || keyboard == 27)
            break;
    }
}

void face_track() {
    
    cv::Ptr<cv::Tracker> tracker = cv::TrackerKCF::create();

    int deviceID = 0;
    int apiID = cv::CAP_ANY;
    cv::VideoCapture cap;
    cap.open(deviceID, apiID);

    cv::CascadeClassifier cascade("haarcascade_frontalface_alt.xml");
    cv::CascadeClassifier eyes_cascade("haarcascade_eye_tree_eyeglasses.xml");
    cv::Rect face_rect;
    cv::Mat ref, gray;

    bool init_tracker = false;

    while (true) {
        cap >> ref;

        if (!init_tracker) {
            cvtColor(ref, gray, cv::COLOR_BGR2GRAY);
            cv::equalizeHist(gray, gray);
            vector<cv::Rect> faces;
            cascade.detectMultiScale(gray, faces);

            if (faces.size() == 0)
                putText(ref, "Cannot detect face", cv::Point(100, 80), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(0, 0, 255), 2);
            else {
                for (size_t i = 0; i < faces.size(); i++) {
                    cv::Mat faceROI = gray(faces[i]);
                    vector<cv::Rect> eyes;
                    eyes_cascade.detectMultiScale(faceROI, eyes);
                    if (eyes.size() != 0) {
                        face_rect = faces[i];
                        break;
                    }
                }

                if (!face_rect.empty()) {
                    tracker->init(ref, face_rect);
                    init_tracker = true;
                }
            }
        }
        else {
            if (tracker->update(ref, face_rect))
                rectangle(ref, face_rect, cv::Scalar(255, 0, 0), 2, 1);
            else
                putText(ref, "Tracking failure detected", cv::Point(100, 80), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(0, 0, 255), 2);
        }

        imshow("Frame", ref);

        int keyboard = cv::waitKey(30);
        if (keyboard == 'q' || keyboard == 27)
            break;
    }
}

int main(int argc, char* argv[])
{
    if (parseCmdArgs(argc, argv))
        return EXIT_FAILURE;
    
    if (segmentation)
    {
        grabCut_method();
        distTrans_method();
    }

    if (br)
    {
        br_method(cv::createBackgroundSubtractorMOG2());
        br_method(cv::createBackgroundSubtractorKNN());
    }

    if (optical_flow)
    {
        vector<cv::Scalar> colors;
        cv::RNG rng;
        for (int i = 0; i < 100; i++)
        {
            int r = rng.uniform(0, 256);
            int g = rng.uniform(0, 256);
            int b = rng.uniform(0, 256);
            colors.push_back(cv::Scalar(r, g, b));
        }

        cv::Mat old_frame, old_gray;
        vector<cv::Point2f> p0, p1;

        capture >> old_frame;
        cvtColor(old_frame, old_gray, cv::COLOR_BGR2GRAY);
        goodFeaturesToTrack(old_gray, p0, 100, 0.3, 7, cv::Mat(), 7, false, 0.04);

        cv::Mat mask = cv::Mat::zeros(old_frame.size(), old_frame.type());

        while (true) {
            cv::Mat frame, frame_gray;
            capture >> frame;
            if (frame.empty())
                break;
            cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);

            vector<uchar> status;
            vector<float> err;
            cv::TermCriteria criteria = cv::TermCriteria((cv::TermCriteria::COUNT)+(cv::TermCriteria::EPS), 10, 0.03);
            calcOpticalFlowPyrLK(old_gray, frame_gray, p0, p1, status, err, cv::Size(15, 15), 2, criteria);

            vector<cv::Point2f> good_new;
            for (uint i = 0; i < p0.size(); i++)
            {
                if (status[i] == 1) {
                    good_new.push_back(p1[i]);
                    line(mask, p1[i], p0[i], colors[i], 2);
                    circle(frame, p1[i], 5, colors[i], -1);
                }
            }

            cv::Mat output;
            add(frame, mask, output);
            imshow("Frame", output);
            int keyboard = cv::waitKey(30);
            if (keyboard == 'q' || keyboard == 27)
                break;

            old_gray = frame_gray.clone();
            p0 = good_new;
        }
    }

    if (tracking)
    {
        simple_track();
    }

    if (face_tracking)
    {
        face_track();
    }

    return 0;
}
