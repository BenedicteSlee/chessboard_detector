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
                                                    tr("Open Image"), "/Users/benedicte/Dropbox/kings/thesis/images",
                                                    tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));

    img_rgb = cv::imread(fileName.toStdString().data());
    if (img_rgb.data){
        //ui->pushButton_2->setEnabled(true);

        // Convert image to graylevel and normalize
        cv::cvtColor(img_rgb, img_gray, CV_RGB2GRAY);
        cv::normalize(img_gray, img_gray, 0, 255, cv::NORM_MINMAX, CV_8UC1);
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    if (ui->UseDefaultImage->isChecked()){
        img_rgb = cv::imread("/Users/benedicte/Dropbox/kings/thesis/images/checkers6.jpg");
        cv::cvtColor(img_rgb, img_gray, CV_RGB2GRAY);
        cv::normalize(img_gray, img_gray, 0, 255, cv::NORM_MINMAX, CV_8UC1);
    }

    cv::Mat img;

    cv::resize(img_gray, img, cv::Size(1000, img_gray.rows * 1000/img_gray.cols));
    ///// CIRCLES
    ///
    ///
    cv::Mat src;

    Preprocess prepcircles = Preprocess(img);
    //src = prepcircles.getCanny();
    img.copyTo(src);
    std::vector<cv::Vec3f> circles;

    /// Apply the Hough Transform to find the circles
    //cv::HoughCircles(src, circles, CV_HOUGH_GRADIENT, 1, src.rows/8, 200, 100, 0, 0 );

    cv::HoughCircles(src, circles, CV_HOUGH_GRADIENT, 1, src.rows/8, 5, 50, 5, 50 );


    if (circles.size() > 0){
        /// Draw the circles detected
        for( size_t i = 0; i < circles.size(); i++ )
        {
            cv::Point2d center((circles[i][0]), (circles[i][1]));
            int radius = cvRound(circles[i][2]);
            // circle center
            circle( src, center, 3, cv::Scalar(0,255,0), -1, 8, 0 );
            // circle outline
            circle( src, center, radius, cv::Scalar(0,0,255), 3, 8, 0 );
        }

        /// Show your results
        cv::namedWindow( "Hough Circle Transform Demo", CV_WINDOW_AUTOSIZE );
        cv::imshow( "Hough Circle Transform Demo", src );
        cv::waitKey(100);

    } else {
        std::cout << "No circles found" << std::endl;
    }


    // Find houghlines
    std::vector<Line> houghlines;
    
    Preprocess prep = Preprocess(img);
    prep.getLines(houghlines);

    prep.showCanny();
    prep.showHoughlines();

    // chessboard detector
    BoardDetector cbd = BoardDetector(img, houghlines);
    Board board = cbd.detect(true);

    // detect pieces based on houghcircles on whole board
    cv::Mat rgb;
    cv::cvtColor(img, rgb, cv::COLOR_GRAY2RGB);
    auto elements = board.getRefs();
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
    cvutils::plotPoints(rgb, centers, 10);

    // Detect pieces based on houghcircles on individual squares
    Points2d centers2;
    for (size_t i = 0; i < elements.size(); i++){
        bool containsPiece = elements[i].containsPiece();
        if (containsPiece){
            centers2.push_back(elements[i].getCenter());
        }
    }
    cvutils::plotPoints(rgb, centers2, 10, cv::Scalar(255,0,0));
}

void MainWindow::on_pushButton_3_clicked()
{

    State state = State::createState(2);
    state.print();
    const State *ps = &state;

    bool isEndOfGame = false;
    int player = 1;
    int count = 0;
    State newstate;
    while (!isEndOfGame && count++ < 1000){
        newstate = checkers::play(ps, player);
        player *= -1; // switch player
        isEndOfGame = newstate.isEndOfGame();
    }
    newstate.print();
    int hei = 1;

    




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

std::vector<State> findmoves(const State& state, int pieceidx){
    std::vector<int> surrs = findsurroundings();
    int currentBest = 0;
    std::vector<int> moves;
    Path currentpath;
    currentpath.start = pieceidx;
    for (int i = 0; i < 4; i++){
        if (surr[i] * piece < 0 && surr[i+4] == 0){
            State newstate(state, pieceidx, outermoves[i]);
            int newpieceidx = pieceIdx + move.first*8 + move.second;
            Path path  = findpath(newstate, newpieceidx, 10, currentpath);
            if (path.captured > currentBest){
                currentBest = path.captured;
                moves = path.moves;
            }
        }
    }
}

Path findpath(State &state, int pieceidx, int depth, Path currentpath){
    if (depth == 0 || state->isEndOfGame()){
        return currentpath;
    }
    std::vector<int> surrs = findsurroundings();
    for (int i = 0; i < 4; i++){
        if (surrs[i] * piece < 0 && surr[i+4] == 0){
            State newstate(state, pieceidx, outermoves[i]);
            int newpieceidx = pieceIdx + move.first*8 + move.second;
            currentpath.moves.push_back(outermoves[i]);
            currentpath.captured++;
            return findpath(newstate, newpieceidx, depth--, currentpath);
        }
    }
}

struct Path{
    int captured;
    std::vector<int> moves;
    int start;
};



