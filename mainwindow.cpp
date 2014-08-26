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

cv::Mat global::image, global::image_gray, global::image_norm, global::image_rgb, global::image_pieces, global::image_hough_mod;
bool global::doDraw;

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


    std::string casen = "casestudy";
    bool saveimages = true;
    global::doDraw = true;
    std::string reportPath = "/Users/benedicte/Dropbox/kings/thesis/report/"+casen+"/";
    //std::string filename =  "Report_" + utils::currentDateTime();

    //if (ui->UseDefaultglobal::image_imp->isChecked()){
    if (!global::image_rgb.data){
        //global::image_rgb = cv::imread("/Users/benedicte/Dropbox/kings/thesis/images/checkers7.jpg");
        //global::image_rgb = cv::imread("/Users/benedicte/Dropbox/kings/thesis/report/boards/green2.jpg");
        //global::image_rgb = cv::imread("/Users/benedicte/Dropbox/kings/thesis/report/pieces/red.jpg");
        global::image_rgb = cv::imread("/Users/benedicte/Dropbox/kings/thesis/report/casestudy/casestudy.jpg");
        //global::image_rgb = cv::imread("/Users/benedicte/Dropbox/kings/thesis/report/"+casen+"/"+casen+".jpg");

        if (global::image_rgb.data){
            cvutils::rotate(global::image_rgb,global::image_rgb,-90);
        } else {
            throw std::invalid_argument("Image could not be read");
        }
    }

    // Set up global image variables
     Preprocess prep;
     Settings::PreprocessSettings settings;


    // chessboard detector
    Lines houghlines;

    Board board; // container for the detected board
    bool tryAgain = true;
    bool boardDetected = false;


    while (tryAgain){
        prep.detectLines(settings);
        prep.getLines(houghlines);
        BoardDetector cbd = BoardDetector(houghlines);
        prep.showCanny();
        prep.showHoughlines();
        try{
            std::cout << "Trying to detect board with blur sigma " << settings.gaussianBlurSigma << std::endl;
            boardDetected = cbd.detect(board, &reportPath);
        } catch(std::exception &e){
        }

        if (boardDetected){
            tryAgain = false;
        } else {
            if (settings.gaussianBlurSigma > 1)
                settings.gaussianBlurSigma -= 1;
            if (settings.gaussianBlurSigma == 1){
                settings.gaussianBlurSize = cv::Size(3,3);
            }
            if (settings.gaussianBlurSize == cv::Size(3,3) && settings.gaussianBlurSigma == 1){
                settings.gaussianBlurSize = cv::Size(1,1);
            }
            if (settings.gaussianBlurSigma == 1 && settings.gaussianBlurSize == cv::Size(1,1) && settings.cannyLow > 4){
                settings.cannyLow -= 4;
            }
            if (settings.gaussianBlurSigma == 1 && settings.gaussianBlurSize == cv::Size(1,1) && settings.cannyLow <= 4){
                throw std::invalid_argument("I give up");
            }
        }
    }


    // save images


    if (saveimages){
        cv::imwrite("/Users/benedicte/Dropbox/kings/thesis/report/"+casen+"/rgb.png", global::image_rgb);
        cv::imwrite("/Users/benedicte/Dropbox/kings/thesis/report/"+casen+"/gray.png", global::image_gray);
        cv::imwrite("/Users/benedicte/Dropbox/kings/thesis/report/"+casen+"/normalized.png", global::image_norm);
        cv::imwrite("/Users/benedicte/Dropbox/kings/thesis/report/"+casen+"/resized.png", global::image);
        cv::imwrite("/Users/benedicte/Dropbox/kings/thesis/report/"+casen+"/canny.png", prep.getCanny());
        cv::imwrite("/Users/benedicte/Dropbox/kings/thesis/report/"+casen+"/hough.png", prep.getHough());
        cv::imwrite("/Users/benedicte/Dropbox/kings/thesis/report/"+casen+"/blurred.png", prep.getBlurred());
    }
    if (saveimages){

        cv::imwrite("/Users/benedicte/Dropbox/kings/thesis/report/"+casen+"/hough_mod.png", global::image_hough_mod);
        board.write("/Users/benedicte/Dropbox/kings/thesis/report/"+casen+"/final.png");
    }


    board.draw();
    cv::destroyAllWindows();
    board.detectPieces();

    if (global::doDraw) board.drawWithPieces();
    board.writeImgWithPiecesToGlobal();
    if (saveimages){
        cv::imwrite("/Users/benedicte/Dropbox/kings/thesis/report/"+casen+"/pieces.png", global::image_pieces);
    }
    // Initial state to feed minimax
    cv::destroyAllWindows();

    State state = board.initState();

    std::cout << "FINISHED" << std::endl;
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




