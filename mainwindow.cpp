#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "opencvbook.cpp"
#include "Line.h"
#include "Chessboard.h"
#include "preprocess.h"
#include "boarddetector.h"
#include "cvutils.h"
#include "typedefs.h"
#include <vector>
#include <numeric>

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
    }
    //cv::namedWindow("Original Image");
    //cv::imshow("Original Image", image);
}

void MainWindow::on_pushButton_2_clicked()
{

    // Find houghlines
    std::vector<Line> houghlines;
    cv::Mat imWithLines;
    Preprocess prep = Preprocess(image);
    prep.getLines(houghlines);

    // chessboard detector
    BoardDetector cbd = BoardDetector(image, houghlines);

    // plot possible squares
    Squares possibleSquares = cbd.getPossibleSquares();

    cv::RNG rng = cv::RNG(1234);
    for (size_t i = 0; i < possibleSquares.size(); ++i) {
        cv::Scalar col = cv::Scalar(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255));
        cv::fillConvexPoly(image, possibleSquares.at(i).getCorners(), col);
        cv::imshow("polys", image);
        cv::waitKey(0);
    }

    // convert to rgb
    cv::Mat img_rgb(imWithLines.size(), CV_8UC3);
    //cvtColor(imWithLines, img_rgb, CV_GRAY2RGB);
    img_rgb = image;

    cv::imshow("polys", image);
    cv::waitKey(0);

    // add lines to image
    for( size_t i = 0; i < houghlines.size(); i++ )
    {
        std::vector<cv::Point> l = houghlines.at(i).points;
        cv::line(img_rgb, l[0], l[1], cv::Scalar(255,0,0), 2, CV_AA);
    }

    // find intersections
    std::vector<cv::Point> points, newpoints;
    cv::Point limits(img_rgb.size[1], img_rgb.size[0]);
    std::vector<double> distances;

    Line::Intersections(houghlines, points, limits);
    Line::RemoveDuplicateIntersections(points,newpoints, distances);

    //std::cout << accumulate(distances.begin(), distances.end(), 0.0) / distances.size() << std::endl;

    // add lines to image
    for( size_t i = 0; i < houghlines.size(); i++ )
    {
        std::vector<cv::Point> l = houghlines.at(i).points;
        cv::line(img_rgb, l[0], l[1], cv::Scalar(255,0,0), 1, CV_AA);
    }

    // add circles to image
    //cv::RNG rng = cv::RNG(1234);
    cv::Scalar col = cv::Scalar(0,255,0);
    for (size_t i=0; i<newpoints.size();i++){
        //cv::Scalar col = cv::Scalar(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255));
        cv::circle(img_rgb, newpoints.at(i),5, col,1);
        //cv::imshow("Intersections", img_rgb);
        //std::cout << newpoints.at(i) << std::endl;
        //cv::waitKey(2);
    }
    cv::imshow("Intersections", img_rgb);
    cv::destroyAllWindows();

}

void MainWindow::on_pushButton_3_clicked()
{
    std::vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    std::cout << vec[0] << "," << vec[1] << std::endl;
}
