#include <vector>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/core/ocl.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/core/types.hpp>
#include <opencv2/ml/ml.hpp>

using namespace std;

bool face_detection = false;
bool bow = false;
string img_path = "";

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

void printUsage(char** argv)
{
    cout <<
        "Week 5\n" << "Usage:\n" << argv[0] << " [Flags] \n"
        "Flags:\n"
        "  -mode (face_detection|BoW)\n"
        "  -img (path to image)\n"
        "\n"
        "Examples:\n"
        "-mode face_detection -img ex_me.jpg\n"
        "-mode BoW\n";
}

int parseCmdArgs(int argc, char** argv)
{
    if (argc == 1)
    {
        printUsage(argv);
        return EXIT_FAILURE;
    }

    bool img_file = false;

    for (int i = 1; i < argc; ++i)
    {
        if (string(argv[i]) == "--help" || string(argv[i]) == "/?")
        {
            printUsage(argv);
            return EXIT_FAILURE;
        }
        else if (string(argv[i]) == "-mode")
        {
            if (string(argv[i + 1]) == "face_detection")
            {
                face_detection = true;
            }
            else if (string(argv[i + 1]) == "BoW")
            {
                bow = true;
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
    }

    if (!(face_detection || bow)) {
        cout << "Please, set -mode" << endl;
        return EXIT_FAILURE;
    }

    if (face_detection && !img_file) {
        cout << "Please, load -img" << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void haar_cascade() {
    cv::CascadeClassifier face_cascade("haarcascade_frontalface_alt.xml");
    cv::CascadeClassifier eyes_cascade("haarcascade_eye_tree_eyeglasses.xml");

    cv::Mat example_pic = cv::imread(img_path);
    cv::resize(example_pic, example_pic, cv::Size(), 0.25, 0.25);

    cv::Mat frame_gray;
    cvtColor(example_pic, frame_gray, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(frame_gray, frame_gray);

    std::vector<cv::Rect> faces;
    face_cascade.detectMultiScale(frame_gray, faces);

    for (size_t i = 0; i < faces.size(); i++)
    {
        cv::Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
        ellipse(example_pic, center, cv::Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, cv::Scalar(255, 0, 255), 4);
        cv::Mat faceROI = frame_gray(faces[i]);

        std::vector<cv::Rect> eyes;
        eyes_cascade.detectMultiScale(faceROI, eyes);
        for (size_t j = 0; j < eyes.size(); j++)
        {
            cv::Point eye_center(faces[i].x + eyes[j].x + eyes[j].width / 2, faces[i].y + eyes[j].y + eyes[j].height / 2);
            int radius = cvRound((eyes[j].width + eyes[j].height) * 0.25);
            circle(example_pic, eye_center, radius, cv::Scalar(255, 0, 0), 4);
        }
    }

    show("Face detection", example_pic);
}

// http://www.vision.caltech.edu/Image_Datasets/Caltech101/
const string DATASET_PATH = "\\101_ObjectCategories\\";
const string IMAGE_EXT = ".jpg";
const int DICT_SIZE = 230;
void readDetectCompute(const string& className, int imageNumbers, int classLable, cv::Mat& allDescriptors, 
                       vector<cv::Mat>& allDescPerImg, vector<int>& allClassPerImg, int& allDescPerImgNum) {
    int count = 0;

    cout << "****** Image loading, Features ******" << endl;
    for (int i = 1; i <= imageNumbers; i++) {
        ostringstream ss;
        cv::Mat grayimg;
        cv::Ptr<cv::SIFT> siftptr;
        siftptr = cv::SIFT::create();

        ss.str("");
        ss << std::setw(4) << std::setfill('0') << i;
        string filename = DATASET_PATH + className + "\\image_" + ss.str() + IMAGE_EXT;
        if (fileExists(filename)) {
            cout << filename << endl;
            cvtColor(cv::imread(filename), grayimg, cv::COLOR_BGR2GRAY);

            vector<cv::KeyPoint> keypoints;
            cv::Mat descriptors;
            siftptr->detectAndCompute(grayimg, cv::Mat(), keypoints, descriptors);
            allDescriptors.push_back(descriptors);
            allDescPerImg.push_back(descriptors);
            allClassPerImg.push_back(classLable);
            allDescPerImgNum++;

            if (count < 3) {
                cv::Mat output;
                drawKeypoints(grayimg, keypoints, output, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
                show("Result [" + ss.str() + IMAGE_EXT + "]", output);
                count++;
            }
        }
        else {
            cout << "File doesn`t exist" << endl;
            break;
        }
    }
}

cv::Mat getDataVector(cv::Mat descriptors, cv::Mat& kCenters) {
    cv::BFMatcher matcher;
    vector<cv::DMatch> matches;
    matcher.match(descriptors, kCenters, matches);

    //Make a Histogram of visual words
    cv::Mat datai = cv::Mat::zeros(1, DICT_SIZE, CV_32F);
    int index = 0;
    for (auto j = matches.begin(); j < matches.end(); j++, index++) {
        datai.at<float>(0, matches.at(index).trainIdx) = datai.at<float>(0, matches.at(index).trainIdx) + 1;
    }
    return datai;
}

bool test(const string& image_path, int classLable, cv::Ptr<cv::ml::SVM> svm, cv::Mat& kCenters) {
    cout << "****** Test ******" << endl;

    if (fileExists(image_path)) {
        cv::Mat gray_img;
        cv::Ptr<cv::SIFT> siftptr;
        siftptr = cv::SIFT::create();

        cvtColor(cv::imread(image_path), gray_img, cv::COLOR_BGR2GRAY);
        vector<cv::KeyPoint> keypoints;
        cv::Mat descriptors;
        siftptr->detectAndCompute(gray_img, cv::noArray(), keypoints, descriptors);
        cv::Mat dvector = getDataVector(descriptors, kCenters);

        if (svm->predict(dvector) == classLable) {
            return true;
        }
    }
    else {
        cout << "File doesn`t exist" << endl;
    }

    return false;
}

void bow_method() {
    cv::Mat allDescriptors;
    vector<cv::Mat> allDescPerImg;
    vector<int> allClassPerImg;
    int allDescPerImgNum = 0;

    readDetectCompute("crayfish", 4, 1, allDescriptors, allDescPerImg, allClassPerImg, allDescPerImgNum);
    readDetectCompute("sunflower", 4, 2, allDescriptors, allDescPerImg, allClassPerImg, allDescPerImgNum);

    int clusterCount = DICT_SIZE;
    int attempts = 5;
    int iterationNumber = 1e4;

    cout << "****** Clustering ******" << endl;
    cv::Mat kCenters, kLabels;
    kmeans(allDescriptors, clusterCount, kLabels, cv::TermCriteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, iterationNumber, 1e-4), attempts, cv::KMEANS_PP_CENTERS, kCenters);

    cout << "****** Histogram ******" << endl;
    cv::Mat inputData;
    cv::Mat inputDataLables;

    for (int i = 0; i < allDescPerImgNum; i++) {
        cv::BFMatcher matcher;
        vector<cv::DMatch> matches;
        matcher.match(allDescPerImg[i], kCenters, matches);

        cv::Mat datai = cv::Mat::zeros(1, DICT_SIZE, CV_32F);
        int index = 0;
        for (auto j = matches.begin(); j < matches.end(); j++, index++) {
            datai.at<float>(0, matches.at(index).trainIdx) = datai.at<float>(0, matches.at(index).trainIdx) + 1;
        }

        inputData.push_back(datai);
        inputDataLables.push_back(cv::Mat(1, 1, CV_32SC1, allClassPerImg[i]));
    }

    cout << "****** SVM ******" << endl;
    cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::create();
    svm->setType(cv::ml::SVM::C_SVC);
    svm->setKernel(cv::ml::SVM::LINEAR);
    svm->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 1e4, 1e-6));

    cv::Ptr<cv::ml::TrainData> td = cv::ml::TrainData::create(inputData, cv::ml::ROW_SAMPLE, inputDataLables);
    svm->train(td);

    string file_test1 = DATASET_PATH + "crayfish\\image_0069.jpg";
    cout << "crayfish\\image_0069.jpg = " << test(file_test1, 1, svm, kCenters) << endl;
}

int main(int argc, char* argv[])
{
    if (parseCmdArgs(argc, argv))
        return EXIT_FAILURE;
    
    if (face_detection)
        haar_cascade();
    // Train
    // https://docs.opencv.org/4.5.2/dc/d88/tutorial_traincascade.html

    if (bow)
        bow_method();

    return 0;
}
