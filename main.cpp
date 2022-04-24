// Face Detection and Monitor Brightness Adujustment
// Mark Downs, Justin Guilarte

/* This program detects your face through the opencv library 
*       if a face is detected, the monitor brightness will be set to 100
*       if a face is not detected, the monitor brightness will gradually decrease to 10
*       face detection works in real time while running the program
*  Assumptions and Details:
*  This program uses the haas cascade algorithm included with the library
*  This program can only affect brightness on windows machines 
*  Must have OpenCV correctly installed in order for program to run 
*/

#include <opencv2\opencv.hpp>

#include <vector>
#include <chrono>
#include <thread>
#include <Windows.h>
#include <sstream>
#include <fstream>
#include <string>

using namespace cv;
using namespace std;

// timer used for turning brightness up or down
struct timer
{
    unsigned long long elapsed_millisecs() const
    {
        using namespace std::chrono;
        return duration_cast<milliseconds>(steady_clock::now() - start_time).count();
    }

    std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
};

int main() {

    double pScale = 4.0;
    timer t;
    //default brightness values to adjust to
    int default_high = 100;
    int default_low = 10;
    CascadeClassifier faceObj;
    faceObj.load("C:\\Users\\mdown\\Documents\\OpenCV Full\\opencv\\etc\\haarcascades\\haarcascade_frontalface_default.xml");

    VideoCapture webcam(0);
    if (!webcam.isOpened()) {
        return -1;
    }
// infinite loop for program to run on
// opens webcam and uses face detection and controls monitor brightness
// stop program through console by pressing any key on keyboard
    for (;;) {
        Mat frame;
        webcam >> frame;

        //a lot of opencv functions only work in grayscale image
        //mat = matrix
        Mat grayscale;
        //convert frame to grayscale
        cvtColor(frame, grayscale, COLOR_BGR2GRAY);

        //rescale frame to improve performance
        resize(grayscale, grayscale, Size(grayscale.size().width / pScale, grayscale.size().height / pScale));

        vector<Rect> faces;
        faceObj.detectMultiScale(grayscale, faces, 1.1, 3, 0, Size(30, 30));
        // if face is not empty gradually increase brightness to 100
        if (faces.empty()) {
           
            //this_thread::sleep_for(chrono::seconds(15));
            for (int i = default_low; i <= default_high; i += 5) {
                int brightness = i + 5;
                if (t.elapsed_millisecs() > 4000 ) {
     
                    system(("powershell (Get-WmiObject -Namespace root/WMI -Class WmiMonitorBrightnessMethods).WmiSetBrightness(1, " + to_string(brightness) + string(")")).c_str());
                    t = {};
                }
            }
            
        }
        // else gradually reduce brightness to default
        else {
            for (int i = default_high; i >= default_low; i -= 5) {
                int brightness = i - 5;
                if (t.elapsed_millisecs() > 4000) {

                    system(("powershell (Get-WmiObject -Namespace root/WMI -Class WmiMonitorBrightnessMethods).WmiSetBrightness(1, " + to_string(brightness) + string(")")).c_str());
                    t = {};
                }
            }
            
        }
        //draws rectangle around face if it is detected
        for (Rect area : faces) {
            //set bgr color
            Scalar drawColor = Scalar(255, 0, 0);
            //make rectangle in frame with top left corner and bottom left corner, and accounting for the scale also
            rectangle(frame, Point(cvRound(area.x * pScale), cvRound(area.y * pScale)),
                Point(cvRound((area.x + area.width - 1) * pScale), cvRound((area.y + area.height - 1) * pScale)), drawColor);
        }

        imshow("Program", frame);
        //closes program if you press any key
        if (waitKey(60) >= 0) {
            break;
        }
    }
    
    return 0;
}
