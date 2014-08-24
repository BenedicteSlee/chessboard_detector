#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

void sharpen(const cv::Mat& image, cv::Mat& result){
    // allocate if necessary
    result.create(global::image.size(), global::image.type());
    std::cout << "Size: " << global::image.size() << std::endl;
    std::cout << "Cols: " << global::image.cols << "\n" << "Rows: " << global::image.rows << std::endl;

    for (int j=1; j<global::image.rows-1; j++){ // for all rows except first and last
        const uchar* previous = global::image.ptr<const uchar>(j-1); // previous row
        const uchar* current = global::image.ptr<const uchar>(j); // current row
        const uchar* next = global::image.ptr<const uchar>(j+1); // next row

        uchar* output = result.ptr<uchar>(j); // output row

        for (int i=1; i<global::image.cols-1; i++){
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

void salt(cv::Mat& image, int n){
    for (int k=0; k<n; k++) {
        int i = qrand() % global::image.cols;
        int j = qrand() % global::image.rows;

        if (global::image.channels() == 1) {

            global::image.at<uchar>(j,i) = 255;
        } else if (global::image.channels() == 3) {

            global::image.at<cv::Vec3b>(j,i)[0] = 255;
            global::image.at<cv::Vec3b>(j,i)[1] = 255;
            global::image.at<cv::Vec3b>(j,i)[2] = 255;
        }
    }
}

void colorReduce(cv::Mat& image, int div=64)
{
    // Obtain beginning and end positions
    cv::MatIterator_<cv::Vec3b> it = global::image.begin<cv::Vec3b>();
    cv::MatIterator_<cv::Vec3b> itend = global::image.end<cv::Vec3b>();

    // Loop over pixels
    while (it!=itend){
        (*it)[0] = (*it)[0]/div*div + div/2;
        (*it)[1] = (*it)[1]/div*div + div/2;
        (*it)[2] = (*it)[2]/div*div + div/2;
        it++;
    }

    int nl = global::image.rows;
    int nc = global::image.cols * global::image.channels(); // total number of elements per line

    for (int j=0; j < nl; j++){
        // get address of row j
        uchar* data  = global::image.ptr<uchar>(j);

        for (int i=0; i < nc; i++){
            data[i] = data[i]/div*div + div/2;
        }
    }
}

