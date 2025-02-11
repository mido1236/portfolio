#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

/**
 * Simple object tracker based on OpenCV. Be sure to include the DLL files at runtime
 * if you're using the Visual Studio based approach
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char **argv) {
    VideoCapture cap(argc > 1 ? stoi(argv[1]) : 0); // Open default webcam
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open webcam" << std::endl;
        return -1;
    }

    Mat frame, hsv, mask;
    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        // Convert to HSV color space
        cvtColor(frame, hsv, COLOR_BGR2HSV);

        // Define color range (adjust for different colors)
        Scalar lower(0, 0, 168); // Lower bound for blue (H, S, V)
        Scalar upper(172, 111, 255); // Upper bound for blue

        // Threshold the image
        inRange(hsv, lower, upper, mask);

        // Find contours
        std::vector<std::vector<Point> > contours;
        findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        // Draw bounding boxes around detected objects
        for (const auto &contour: contours) {
            if (contourArea(contour) > 500) {
                // Filter small noise
                Rect bbox = boundingRect(contour);
                rectangle(frame, bbox, Scalar(0, 255, 0), 2);
            }
        }

        // Display results
        imshow("Tracked Object", frame);
        imshow("Mask", mask);

        // Exit on 'q' key
        if (waitKey(30) == 'q') break;
    }

    cap.release();
    destroyAllWindows();
    return 0;
}
