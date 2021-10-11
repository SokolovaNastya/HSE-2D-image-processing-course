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
#include "opencv2/stitching.hpp"

using namespace std;

bool matching = false;
string match_type = "";

bool stitching = false;
bool stitch_custom = false;

vector<cv::Mat> imgs;

const double kDistanceCoef = 4.0;
const int kMaxMatchingSize = 50;

void printUsage(char** argv)
{
    cout <<
        "Images stitcher.\n\n" << "Usage :\n" << argv[0] << " [Flags] img1 img2 [...imgN]\n\n"
        "Flags:\n"
        "  --mode (panorama|matching)\n"
        "      Determines configuration\n"
        "  --stitch_custom\n"
        "      Applies custom stitching implementation\n"
        "  --matchType (sift|orb)\n"
        "      Determines type of matching\n"
        "Example usage of stitching :\n" << argv[0] << "--mode panorama --stitch_custom bridge1.jpg bridge2.jpg\n"
        "Example usage of stitching :\n" << argv[0] << "--mode panorama bridge1.jpg bridge2.jpg\n"
        "Example usage of stitching :\n" << argv[0] << "--mode panorama leuvenA.jpg leuvenB.jpg\n"
        "Example usage of matching :\n" << argv[0] << "--mode matching --matchType sift match1.jpg match2.jpg\n";
}

int parseCmdArgs(int argc, char** argv)
{
    if (argc == 1)
    {
        printUsage(argv);
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; ++i)
    {
        if (string(argv[i]) == "--help" || string(argv[i]) == "/?")
        {
            printUsage(argv);
            return EXIT_FAILURE;
        }
        else if (string(argv[i]) == "--mode")
        {
            if (string(argv[i + 1]) == "panorama")
            {
                stitching = true;
            }
            else if (string(argv[i + 1]) == "matching")
            {
                matching = true;
            }
            else
            {
                cout << "Bad --mode flag value" << endl;
                return EXIT_FAILURE;
            }

            i++;
        }
        else if (string(argv[i]) == "--stitch_custom")
        {
            if (stitching)
            {
                stitch_custom = true;
            }
            else
            {
                cout << "Need to define --mode" << endl;
                return EXIT_FAILURE;
            }
        }
        else if (string(argv[i]) == "--matchType")
        {
            if (matching)
            {
                match_type = string(argv[i + 1]);

                if (match_type.compare("sift") != 0
                    && match_type.compare("orb") != 0)
                {
                    cout << "Bad --matchType flag value" << endl;
                    return EXIT_FAILURE;
                }

                i++;
            }
            else
            {
                cout << "Need to define --mode" << endl;
                return EXIT_FAILURE;
            }
        }
        else
        {
            cv::Mat img = cv::imread(cv::samples::findFile(argv[i]));
            if (img.empty())
            {
                cout << "Can't read image '" << argv[i] << endl;
                return EXIT_FAILURE;
            }

            imgs.push_back(img);
        }
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

void detect_and_compute(string type, cv::Mat& img, vector<cv::KeyPoint>& kpts, cv::Mat& desc) {

    if (match_type.compare("sift") == 0)
    {
        auto detector = cv::SIFT::create();
        detector->detectAndCompute(img, cv::Mat(), kpts, desc);
    }
    else if (match_type.compare("orb") == 0)
    {
        auto detector = cv::ORB::create();
        detector->detectAndCompute(img, cv::Mat(), kpts, desc);
    }
    else
    {
        cout << "Need to define --matchType" << endl;
        exit(0);
    }
}

void match(cv::Mat& desc1, cv::Mat& desc2, vector<cv::DMatch>& matches) {
    matches.clear();

    // KNN
    cv::BFMatcher desc_matcher(cv::NORM_L2, true);
    vector< vector<cv::DMatch> > vmatches;
    desc_matcher.knnMatch(desc1, desc2, vmatches, 1);

    for (int i = 0; i < static_cast<int>(vmatches.size()); ++i) {
        if (!vmatches[i].size()) {
            continue;
        }
        matches.push_back(vmatches[i][0]);
    }

    std::sort(matches.begin(), matches.end());

    while (matches.front().distance * kDistanceCoef < matches.back().distance) {
        matches.pop_back();
    }

    while (matches.size() > kMaxMatchingSize) {
        matches.pop_back();
    }
}

void findKeyPointsHomography(vector<cv::KeyPoint>& kpts1, vector<cv::KeyPoint>& kpts2, vector<cv::DMatch>& matches, vector<char>& match_mask) {
    if (static_cast<int>(match_mask.size()) < 3) {
        return;
    }

    vector<cv::Point2f> pts1;
    vector<cv::Point2f> pts2;

    for (int i = 0; i < static_cast<int>(matches.size()); ++i) {
        pts1.push_back(kpts1[matches[i].queryIdx].pt);
        pts2.push_back(kpts2[matches[i].trainIdx].pt);
    }

    cv::findHomography(pts1, pts2, cv::RANSAC, 4, match_mask);
}

void removeBlackPortion(vector<cv::Point>& nonBlackList, cv::Mat& result)
{
    nonBlackList.reserve(result.rows * result.cols);

    for (int j = 0; j < result.rows; ++j)
    {
        for (int i = 0; i < result.cols; ++i)
        {
            if (result.at<cv::Vec3b>(j, i) != cv::Vec3b(0, 0, 0))
            {
                nonBlackList.push_back(cv::Point(i, j));
            }
        }
    }
}

int main(int argc, char* argv[])
{
    if (parseCmdArgs(argc, argv))
        return EXIT_FAILURE;

    if (matching)
    {
        cv::Mat img1 = imgs[0];
        cv::Mat img2 = imgs[1];
        vector<cv::KeyPoint> kpts1, kpts2;
        cv::Mat desc1, desc2;

        detect_and_compute(match_type, img1, kpts1, desc1);
        detect_and_compute(match_type, img2, kpts2, desc2);

        vector<cv::DMatch> matches;
        match(desc1, desc2, matches);

        vector<char> match_mask(matches.size(), 1);
        findKeyPointsHomography(kpts1, kpts2, matches, match_mask);

        cv::Mat out_matching;
        cv::drawMatches(img1, kpts1, img2, kpts2, matches, out_matching, cv::Scalar::all(-1),
            cv::Scalar::all(-1), match_mask, cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

        show("matching_result", out_matching);
        
        cout << "matching completed successfully" << endl;
    }
    else
    {
        if (!stitch_custom)
        {
            // opencv_stitching451d.lib
            cv::Mat pano;
            cv::Ptr<cv::Stitcher> stitcher = cv::Stitcher::create(cv::Stitcher::PANORAMA);
            cv::Stitcher::Status status = stitcher->stitch(imgs, pano);

            if (status != cv::Stitcher::OK)
            {
                cout << "Can't stitch images, error code = " << int(status) << endl;
                return EXIT_FAILURE;
            }

            show("pano_result", pano);

            cout << "stitching completed successfully" << endl;
        }
        else
        {
            cv::Mat img1 = imgs[0];
            cv::Mat img2 = imgs[1];
            vector<cv::KeyPoint> kpts1, kpts2;
            cv::Mat desc1, desc2;

            cv::resize(img2, img2, img1.size());

            match_type = "sift";
            detect_and_compute(match_type, img1, kpts1, desc1);
            detect_and_compute(match_type, img2, kpts2, desc2);

            cv::FlannBasedMatcher matcher;
            vector<cv::DMatch> matches;
            matcher.match(desc1, desc2, matches);

            double max_dist = 0;
            double min_dist = 100;

            for (int i = 0; i < desc1.rows; i++)
            {
                double dist = matches[i].distance;
                if (dist < min_dist) min_dist = dist;
                if (dist > max_dist) max_dist = dist;
            }

            printf("-- Max dist : %f \n", max_dist);
            printf("-- Min dist : %f \n", min_dist);

            // Use only "good" matches (i.e. whose distance is less than 3 X min_dist )
            std::vector<cv::DMatch> good_matches;

            for (int i = 0; i < desc1.rows; i++)
            {
                if (matches[i].distance < 3 * min_dist)
                {
                    good_matches.push_back(matches[i]);
                }
            }
            std::vector<cv::Point2f> good_kpts1;
            std::vector<cv::Point2f> good_kpts2;

            for (int i = 0; i < good_matches.size(); i++)
            {
                good_kpts1.push_back(kpts1[good_matches[i].queryIdx].pt);
                good_kpts2.push_back(kpts2[good_matches[i].trainIdx].pt);
            }

            cv::Mat H = findHomography(good_kpts1, good_kpts2, cv::RANSAC);

            cv::Mat result;
            warpPerspective(img1, result, H, cv::Size(img1.cols + img2.cols, img1.rows));
            cv::Mat half(result, cv::Rect(0, 0, img2.cols, img2.rows));
            img2.copyTo(half);

            vector<cv::Point> nonBlackList;
            removeBlackPortion(nonBlackList, result);

            cv::Rect bb = cv::boundingRect(nonBlackList);

            show("pano_result", result(bb));

            cout << "stitching completed successfully" << endl;
        }
    }
    return EXIT_SUCCESS;
}
