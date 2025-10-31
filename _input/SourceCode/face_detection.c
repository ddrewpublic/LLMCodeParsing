// File: face_detection.c

#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>

using namespace cv;

// Function to detect faces in the given image
void detectFaces(Mat img) {
    // Load the pre-trained Haar Cascade classifier for face detection
    CascadeClassifier face_cascade;
    if (!face_cascade.load("haarcascade_frontalface_default.xml")) {
        fprintf(stderr, "Error loading cascade classifier\n");
        return;
    }

    // Convert the image to grayscale for better performance
    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);
    equalizeHist(gray, gray);

    // Vector to hold the detected faces
    std::vector<Rect> faces;
    // Detect faces in the image
    face_cascade.detectMultiScale(gray, faces, 1.1, 3, 0, Size(30, 30));

    // Draw rectangles around detected faces
    for (size_t i = 0; i < faces.size(); i++) {
        rectangle(img, faces[i], Scalar(255, 0, 0), 2);
    }

    // Display the result
    imshow("Detected Faces", img);
    waitKey(0);
}

int main(int argc, char** argv) {
    // Check if the user provided the image file path
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <image_path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Load the input image
    Mat img = imread(argv[1]);
    if (img.empty()) {
        fprintf(stderr, "Error loading image: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    // Call the face detection function
    detectFaces(img);

    return EXIT_SUCCESS;
}
