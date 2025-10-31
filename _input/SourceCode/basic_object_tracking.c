// File: basic_object_tracking.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <opencv2/opencv.hpp>

void process_frame(cv::Mat frame, cv::Ptr<cv::BackgroundSubtractor> bg_subtractor) {
    cv::Mat fg_mask;
    bg_subtractor->apply(frame, fg_mask);

    // Find contours of the moving objects
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(fg_mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Draw bounding boxes around detected objects
    for (size_t i = 0; i < contours.size(); i++) {
        if (cv::contourArea(contours[i]) > 500) { // Filter small objects
            cv::Rect bounding_box = cv::boundingRect(contours[i]);
            cv::rectangle(frame, bounding_box, cv::Scalar(0, 255, 0), 2);
        }
    }

    // Display the frame with detected objects
    cv::imshow("Object Tracking", frame);
}

int main(int argc, char** argv) {
    // Check for valid command line arguments
    if (argc < 2) {
        printf("Usage: %s <video_file>\n", argv[0]);
        return -1;
    }

    // Open video file
    cv::VideoCapture cap(argv[1]);
    if (!cap.isOpened()) {
        printf("Error: Could not open video.\n");
        return -1;
    }

    // Create background subtractor
    cv::Ptr<cv::BackgroundSubtractor> bg_subtractor = cv::createBackgroundSubtractorMOG2();

    // Main loop to process video frames
    cv::Mat frame;
    while (true) {
        cap >> frame; // Capture frame-by-frame
        if (frame.empty()) break; // Exit if no more frames

        process_frame(frame, bg_subtractor);

        // Break the loop if the user presses 'q'
        if (cv::waitKey(30) == 'q') break;
    }

    // Release resources and close windows
    cap.release();
    cv::destroyAllWindows();
    return 0;
}
