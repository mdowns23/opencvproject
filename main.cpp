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
    int default_high = 100;
    int default_low = 10;
    CascadeClassifier faceObj;
    faceObj.load("C:\\Users\\mdown\\Documents\\OpenCV Full\\opencv\\etc\\haarcascades\\haarcascade_frontalface_default.xml");

    VideoCapture webcam(0);
    if (!webcam.isOpened()) {
        return -1;
    }

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
        else {
            for (int i = default_high; i >= default_low; i -= 5) {
                int brightness = i - 5;
                if (t.elapsed_millisecs() > 4000) {

                    system(("powershell (Get-WmiObject -Namespace root/WMI -Class WmiMonitorBrightnessMethods).WmiSetBrightness(1, " + to_string(brightness) + string(")")).c_str());
                    t = {};
                }
            }
            
        }
        for (Rect area : faces) {
            //set bgr color
            Scalar drawColor = Scalar(255, 0, 0);
            //make rectangle in frame with top left corner and bottom left corner, and accounting for the scale also
            rectangle(frame, Point(cvRound(area.x * pScale), cvRound(area.y * pScale)),
                Point(cvRound((area.x + area.width - 1) * pScale), cvRound((area.y + area.height - 1) * pScale)), drawColor);
        }

        imshow("Program", frame);

        if (waitKey(60) >= 0) {
            break;
        }
    }
    
    return 0;
}
