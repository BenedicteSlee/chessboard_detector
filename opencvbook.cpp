#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

void sharpen(const cv::Mat& image, cv::Mat& result){
    // allocate if necessary
    result.create(image.size(), image.type());
    std::cout << "Size: " << image.size() << std::endl;
    std::cout << "Cols: " << image.cols << "\n" << "Rows: " << image.rows << std::endl;

    for (int j=1; j<image.rows-1; j++){ // for all rows except first and last
        const uchar* previous = image.ptr<const uchar>(j-1); // previous row
        const uchar* current = image.ptr<const uchar>(j); // current row
        const uchar* next = image.ptr<const uchar>(j+1); // next row

        uchar* output = result.ptr<uchar>(j); // output row

        for (int i=1; i<image.cols-1; i++){
            *output = cv::saturate_cast<uchar>(5*current[i]-current[i-1]-current[i+1]-previous[i]-next[i]);
            output++;

        }
    }

    // set unprocessed pixels to 0
    result.row(0).setTo(cv::Scalar(0));
    result.row(result.rows-1).setTo(cv::Scalar(0));
    result.col(0).setTo(cv::Scalar(0));
    result.col(result.cols-1).setTo(cv::Scalar(0));
}

void salt(cv::Mat &image, int n){
    for (int k=0; k<n; k++) {
        int i = qrand() % image.cols;
        int j = qrand() % image.rows;

        if (image.channels() == 1) {

            image.at<uchar>(j,i) = 255;
        } else if (image.channels() == 3) {

            image.at<cv::Vec3b>(j,i)[0] = 255;
            image.at<cv::Vec3b>(j,i)[1] = 255;
            image.at<cv::Vec3b>(j,i)[2] = 255;
        }
    }
}

void colorReduce(cv::Mat& image, int div=64)
{
    // Obtain beginning and end positions
    cv::MatIterator_<cv::Vec3b> it = image.begin<cv::Vec3b>();
    cv::MatIterator_<cv::Vec3b> itend = image.end<cv::Vec3b>();

    // Loop over pixels
    while (it!=itend){
        (*it)[0] = (*it)[0]/div*div + div/2;
        (*it)[1] = (*it)[1]/div*div + div/2;
        (*it)[2] = (*it)[2]/div*div + div/2;
        it++;
    }

    int nl = image.rows;
    int nc = image.cols * image.channels(); // total number of elements per line

    for (int j=0; j < nl; j++){
        // get address of row j
        uchar* data  = image.ptr<uchar>(j);

        for (int i=0; i < nc; i++){
            data[i] = data[i]/div*div + div/2;
        }
    }
}

