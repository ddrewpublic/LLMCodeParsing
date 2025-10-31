// File: basic_pose_estimation.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <opencv2/opencv.hpp>

using namespace cv;

// Function to detect keypoints and draw them on the image
void detectAndDrawKeypoints(Mat& img) {
    // Convert to grayscale
    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);

    // Detect keypoints using ORB
    Ptr<ORB> orb = ORB::create();
    std::vector<KeyPoint> keypoints;
    orb->detect(gray, keypoints);

    // Draw keypoints on the image
    Mat output;
    drawKeypoints(img, keypoints, output, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
    
    // Show the output image with keypoints
    imshow("Keypoints", output);
}

int main(int argc, char** argv) {
    // Open the default camera
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        fprintf(stderr, "Error: Could not open the camera.\n");
        return EXIT_FAILURE;
    }

    // Set up window for display
    namedWindow("Keypoints", WINDOW_AUTOSIZE);

    // Main loop to capture frames
    while (true) {
        Mat frame;
        cap >> frame; // Capture a new frame

        if (frame.empty()) {
            fprintf(stderr, "Error: No captured frame.\n");
            break;
        }

        // Detect and draw keypoints
        detectAndDrawKeypoints(frame);

        // Exit on 'q' key press
        if (waitKey(30) == 'q') {
            break;
        }
    }

    // Release the camera and close windows
    cap.release();
    destroyAllWindows();

    return EXIT_SUCCESS;
}
