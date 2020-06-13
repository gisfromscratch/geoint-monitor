#include <QtTest>

// add necessary includes here

class GDELTTestSuite : public QObject
{
    Q_OBJECT

public:
    GDELTTestSuite();
    ~GDELTTestSuite();

private slots:
    void test_case1();

};

GDELTTestSuite::GDELTTestSuite()
{

}

GDELTTestSuite::~GDELTTestSuite()
{

}

void GDELTTestSuite::test_case1()
{

}

QTEST_APPLESS_MAIN(GDELTTestSuite)

#include "tst_gdelttestsuite.moc"
