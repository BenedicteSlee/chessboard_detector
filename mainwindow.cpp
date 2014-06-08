#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "opencvbook.cpp"
#include "Line.h"
#include "Chessboard.h"
#include "preprocess.h"
#include <vector>

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

    std::vector<Line> houghlines;
    cv::Mat imWithLines;
    Preprocess prep = Preprocess();
    prep.getLines(image, imWithLines, houghlines);

    std::vector<cv::Point> points;
    Line::Intersections(houghlines, points);

    std::cout << "Finished" << std::endl;

    cv::Mat img_rgb(imWithLines.size(), CV_8UC3);
    cvtColor(imWithLines, img_rgb, CV_GRAY2RGB);

    for( size_t i = 0; i < houghlines.size(); i++ )
    {
        std::vector<cv::Point> l = houghlines.at(i).points;
        cv::line(img_rgb, l[0], l[1], cv::Scalar(255,0,0), 2, CV_AA);
    }

    for (size_t i=0; i<points.size();i++)
        cv::circle(img_rgb, points.at(i),3,cv::Scalar(0,255,0),2);

    cv::imshow("Intersections", img_rgb);
    cv::waitKey(0);

    //double duration = static_cast<double>(cv::getTickCount());
    //duration = static_cast<double>(cv::getTickCount()) - duration;
    //duration = duration / cv::getTickFrequency(); // get elapsed time
    //std::cout << "Duration: " << duration << std::endl;

    // EMBEDD IMAGE IN GUI (NOT WORKING)
    // Convert to QImage to be able to display
    // cv::cvtColor(image,image, CV_BayerRG2RGB);
    //QImage img = QImage((const unsigned char*)(image.data),image.cols,image.rows,QImage::Format_RGB888);

    // Display on lable
    //ui->label->setPixmap(QPixmap::fromImage(img));
    // Resize label to fit image
    //ui->label->resize(ui->label->pixmap()->size());
}

void MainWindow::on_pushButton_3_clicked()
{
    std::vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    std::cout << vec[0] << "," << vec[1] << std::endl;

}
