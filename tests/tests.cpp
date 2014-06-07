#include <QtTest/QTest>
#include <vector>
#include <opencv2/opencv.hpp>
#include "Line.h"
#include "Chessboard.h"

class tests: public QObject
{
    Q_OBJECT
private slots:
    void Line_test();
    void Square_test();
    /*
    void initTestCase()
    { qDebug("called before everything else"); }
    void myFirstTest()
    { QVERIFY(1 == 1); }
    void mySecondTest()
    { QVERIFY(1 != 2); }
    void cleanupTestCase()
    { qDebug("called after myFirstTest and mySecondTest"); }
    */
};

void tests::Square_test()
{
    // draw test
    Square square = Square();
    square.setCorners(cv::Point(0,0), cv::Point(100,0), cv::Point(100,100),cv::Point(0,100));
    square.draw();
}

void tests::Line_test(){
    cv::Point p1, p2, p3, p4;
    p1.x = 0;
    p1.y = 0;
    p2.x = 1;
    p2.y = 1;

    p3.x = -1;
    p3.y = 1;
    p4.x = 0;
    p4.y = 0;

    Line testline = Line(p1, p2);

    Line otherline = Line(p3,p4);
    cv::Point intersection;
    testline.Intersection(otherline, intersection);

    QVERIFY(testline.slope == 1);
    QVERIFY(testline.yIntercept == 0);
    QVERIFY(intersection.x == 0);
    QVERIFY(intersection.y == 0);
}

QTEST_MAIN(tests)
#include "tests.moc"


