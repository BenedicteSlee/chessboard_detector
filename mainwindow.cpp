#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "opencvbook.cpp"
#include "Line.h"
#include "square.h"
#include "preprocess.h"
#include "boarddetector.h"
#include "cvutils.h"
#include "typedefs.h"
#include <vector>
#include <numeric>
#include <math.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButton_2->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Image"), "/Users/benedicte/Dropbox/kings/thesis/images",
                                                    tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));

    image = cv::imread(fileName.toStdString().data());
    if (image.data){
        ui->pushButton_2->setEnabled(true);

        // Convert image to graylevel and normalize
        cv::cvtColor(image, img_gray, CV_RGB2GRAY);
        cv::normalize(img_gray, img_gray, 0, 255, cv::NORM_MINMAX, CV_8UC1);
    }
}

void MainWindow::on_pushButton_2_clicked()
{

    // Find houghlines
    std::vector<Line> houghlines;
    
    Preprocess prep = Preprocess(image);
    prep.getLines(houghlines);

    // chessboard detector
    BoardDetector cbd = BoardDetector(image, houghlines);

    // add lines to image
    //cv::Mat img_rgb(image.size(), CV_8UC3);
    //cv::cvtColor(image, img_rgb, CV_GRAY2RGB);

    cv::Mat img_rgb = image;
    Lines hlinesSorted = cbd.get_hlinesSorted();
    Lines vlinesSorted = cbd.get_vlinesSorted();

    for( size_t i = 0; i < hlinesSorted.size(); i++ )
    {
        std::vector<cv::Point> l = hlinesSorted.at(i).points;
        cv::line(img_rgb, l[0], l[1], cv::Scalar(255,0,0), 1, CV_AA);
        cv::imshow("lines", img_rgb);
        cv::waitKey(0);
    }
    for( size_t i = 0; i < vlinesSorted.size(); i++ )
    {
        std::vector<cv::Point> l = vlinesSorted.at(i).points;
        cv::line(img_rgb, l[0], l[1], cv::Scalar(255,0,0), 1, CV_AA);
        cv::imshow("lines", img_rgb);
        cv::waitKey(0);
    }

    // get mean color
    Squares possibleSquares = cbd.get_PossibleSquares();
    std::vector<int> meancols;
    int meancol = 0;
    for (size_t i = 0; i < possibleSquares.size(); ++i) {
        meancols.push_back(possibleSquares[i].get_meanGray());
        std::cout << possibleSquares[i].get_meanGray() << std::endl;
        meancol += possibleSquares[i].get_meanGray() / (double) possibleSquares.size();
    }

    // create binary image
    cv::Mat binary;
    cv::threshold(img_gray, binary, 90, 255, 3); // TODO threshold hardcoded to 90. Base on mean color of squares?
    cv::imshow("binary", binary);
    cv::waitKey(0);

    

    /* //PLOT POSSIBLE SQUARES
    cv::RNG rng = cv::RNG(1234);
    for (size_t i = 0; i < possibleSquares.size(); ++i) {
        cv::Scalar col = cv::Scalar(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255));
        cv::fillConvexPoly(image, possibleSquares.at(i).getCorners(), col);
        cv::imshow("poly", image);
        cv::waitKey(1);
    }
    */


 /*
    // add circles to image
    //cv::RNG rng = cv::RNG(1234);
    cv::Scalar col = cv::Scalar(0,255,0);
    for (size_t i=0; i<points.size();i++){
        //cv::Scalar col = cv::Scalar(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255));
        cv::circle(img_rgb, points.at(i),5, col,1);
        cv::imshow("Intersections", img_rgb);
        //std::cout << newpoints.at(i) << std::endl;
        //cv::waitKey(2);
    }
    cv::imshow("Intersections", img_rgb);
   // cv::destroyAllWindows();
   */
}

void MainWindow::on_pushButton_3_clicked()
{
    cv::Point p1(0,0);
    cv::Point p2(0,1);
    cv::Point p3(1,1);
    cv::Point p4(1,0);

    Points points;
    points.push_back(p1);
    points.push_back(p2);
    points.push_back(p3);
    points.push_back(p4);

    Points check = points;

    std::sort(points.begin(), points.end(), cvutils::pointIsLess);

    std::vector<std::pair<int, double> > vec;
    std::pair<int, double> v1(1,2.0);
    std::pair<int,double> v2(2,1.0);

    vec.push_back(v1);
    vec.push_back(v2);

    bool check1 = cvutils::pairIsLess(v2, v1);

    std::cout << check << std::endl;
}
