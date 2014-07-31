#ifndef TESTING_H
#define TESTING_H

#include <QtTest/QtTest>

class test_minimax: public QObject
{
    Q_OBJECT
public slots:
    void test_multimoves();
};

#endif // TESTING_H
