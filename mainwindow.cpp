#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "opencvbook.cpp"
#include "Line.h"
#include "Chessboard.h"
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
    cv::namedWindow("Original Image");
    cv::imshow("Original Image", image);
}

void MainWindow::on_pushButton_2_clicked()
{

    //double duration = static_cast<double>(cv::getTickCount());

    std::cout << "Button 2" << std::endl;


    //cv::namedWindow("Processed image");
    //cv::imshow("Processed image", result);

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
