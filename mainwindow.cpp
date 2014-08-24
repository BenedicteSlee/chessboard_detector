#include <iostream>
#include <fstream>
#include <ctime>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "opencvbook.cpp"
#include "Line.h"
#include "square.h"
#include "preprocess.h"
#include "boarddetector.h"
#include "cvutils.h"
#include "typedefs.h"
#include "matrix.h"
#include <vector>
#include <numeric>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include "state.h"
#include "minimax.h"
#include "utils.h"
#include "global.h"

cv::Mat global::image, global::image_gray, global::image_norm, global::image_rgb;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //ui->pushButton_2->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open image"), "/Users/benedicte/Dropbox/kings/thesis/images",
                                                    tr("image Files (*.png *.jpg *.jpeg *.bmp)"));

    global::image_rgb = cv::imread(fileName.toStdString().data());
}

void MainWindow::on_pushButton_2_clicked()
{
    //std::string filename =  "Report_" + utils::currentDateTime();

    //if (ui->UseDefaultglobal::image_imp->isChecked()){
    if (!global::image_rgb.data){
        global::image_rgb = cv::imread("/Users/benedicte/Dropbox/kings/thesis/images/checkers7.jpg");
        //global::image_rgb = cv::imread("/Users/benedicte/Dropbox/kings/thesis/report/boards/green2.jpg");
        if (global::image_rgb.data){
            cvutils::rotate(global::image_rgb,global::image_rgb,-90);
        } else {
            throw std::invalid_argument("Image could not be read");
        }
    }

    // Set up global image variables
     Preprocess prep;
     Settings::PreprocessSettings settings;
     prep.detectLines(settings);

    // chessboard detector
    Lines houghlines;
    prep.getLines(houghlines);
    prep.showCanny();
    prep.showHoughlines();
    BoardDetector cbd = BoardDetector(houghlines); // set up a board detector
    Board board; // container for the detected board
    bool tryAgain = true;

    std::string reportPath = "/Users/benedicte/Dropbox/kings/thesis/report/case1/";
    while (tryAgain){
        //prep.showCanny();
        //prep.showHoughlines();
        bool boardDetected = cbd.detect(board, false, &reportPath);
        if (boardDetected){
            tryAgain = false;
        } else {
            if (settings.gaussianBlurSigma == 1){
                settings.gaussianBlurSize = cv::Size(3,3);
            }
            if (settings.gaussianBlurSize == cv::Size(3,3)){
                settings.gaussianBlurSize = cv::Size(1,1);
            }
            if (settings.gaussianBlurSigma == 1 && settings.gaussianBlurSize == cv::Size(1,1)){
                throw("I give up");
            }
            settings.gaussianBlurSigma = 1;
        }
    }


    // save global::images
    /*
    bool saveglobal::images = false;
    if (saveglobal::images){
        cv::imwrite("/Users/benedicte/Dropbox/kings/thesis/report/case1/rgb.png", img_rgb);
        cv::imwrite("/Users/benedicte/Dropbox/kings/thesis/report/case1/gray.png", img_gray);
        cv::imwrite("/Users/benedicte/Dropbox/kings/thesis/report/case1/normalized.png", img_norm);
        cv::imwrite("/Users/benedicte/Dropbox/kings/thesis/report/case1/resized.png", img);
        cv::imwrite("/Users/benedicte/Dropbox/kings/thesis/report/case1/canny.png", prep.getCanny());
        cv::imwrite("/Users/benedicte/Dropbox/kings/thesis/report/case1/hough.png", prep.getHough());
        cv::imwrite("/Users/benedicte/Dropbox/kings/thesis/report/case1/blurred.png", prep.getBlurred());
    }
    if (saveglobal::images){
        cbd.printHoughAfterCategorization("/Users/benedicte/Dropbox/kings/thesis/report/case1/hough2.png");
        board.write("/Users/benedicte/Dropbox/kings/thesis/report/case1/final.png");
    }
    */


    // detect pieces based on houghcircles on whole board
    //cv::cvtColor(img, rgb, cv::COLOR_GRAY2RGB);
    /*
    auto elements = board.getElementRefs();
    std::vector<int> squaresWithPiece;
    Points2d centers;
    for (size_t i = 0; i < circles.size(); i++){
        auto x = circles[i][0];
        auto y = circles[i][1];
        centers.push_back(cv::Point2d(x,y));
        cv::Point2d point(x, y);
        auto it2 = std::find_if(elements.begin(), elements.end(), [&](const Square& element){return element.containsPoint(point);});
        size_t dist = std::distance(elements.begin(), it2);
        if (dist < elements.size())
            squaresWithPiece.push_back(dist);
    }
    std::sort(squaresWithPiece.begin(), squaresWithPiece.end());
    //cvutils::plotPoints(rgb, centers, 10);
    */

    /*
    // Detect pieces based on houghcircles on individual squares
    auto elements = board.getElementRefs();
    Points2d centers2;
    for (size_t i = 0; i < elements.size(); i++){
        Square square = elements[i];
        cv::Vec3i circle;
        bool containsPiece = square.detectPieceWithHough(circle);
        if (containsPiece){
            centers2.push_back(elements[i].getCenter());
        }
    }
    */

    cv::destroyAllWindows();
    board.detectPieces();

    board.drawWithPieces();

    // Initial state to feed minimax
    cv::destroyAllWindows();

    State state = board.initState();

    // WRITE REPORTS
}

void MainWindow::on_pushButton_3_clicked()
{

    State state = State::createState(4);
    state.print();

    int player = 1; // get as input from user

    std::vector<State> moves;
    moves = state.findMovesForPiece(1);

    for (size_t i = 0; i < moves.size(); i++){
        std::cout << "Move " << i << std::endl;
        moves[i].print();
    }

    State newstate = checkers::play(state, player);
    int score = checkers::evaluate(newstate, player);
    newstate.print();
    if (score < 0)
        std::cout << "White wins" << std::endl;
    else if (score > 0)
        std::cout << "Black wins" << std::endl;
    else
        std::cout << "Its a tie!" << std::endl;







    /*
    matrix<int> m1, m2, m3, m4, m5;

    std::vector<int> row{1, 1, 1};
    m1.appendRow(row);
    m2.appendRow(row);

    m3 = m1 + m2;
    std::vector<int> v{0,1,2,3,4};
    std::vector<int> vv(&v[1], &v[5]);
    for (size_t i = 0; i < vv.size(); i++){
        std::cout << vv[i] << std::endl;
    }

    std::vector<int> col{2, 2, 2};
    m4.appendCol(col);
    m4.appendCol(col);
    m4.prependCol(row);

    m5.appendRow(row);
    m5.appendRow(row);
    m5.prependRow(col);
    */
}

void MainWindow::on_pushButton_4_clicked()
{

}




