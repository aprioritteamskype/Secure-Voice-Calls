#include "RecorderTests.h"
#include "recorder.h"

void RecorderTest::testWriteData()
{
    Recorder recorder;
    int testSize = 10;

    recorder.writeData("987654321",testSize);

    char outputData[testSize];// place where data shoud be writen
    recorder.getDataFromBuff(outputData,testSize);

    QCOMPARE(outputData,"987654321");
}

void RecorderTest::testBufferMaxWritingPos()
{
    Recorder recorder(18);
    int testSize = 16;


    recorder.writeData("000000000000000",testSize);
    char outputData[testSize];
    recorder.getDataFromBuff(outputData,testSize);
    QCOMPARE(outputData,"000000000000000");


      recorder.writeData("111111111111111",testSize);
      recorder.getDataFromBuff(outputData,testSize);
      QCOMPARE(outputData,"111111111111111");
}

void RecorderTest::testBufferSize()
{
//    Player player(100);
//    int buffSize = player.getBuffSize();
//    QCOMPARE(buffSize,100);
}

void RecorderTest::testBytesToGetOverflow()
{
//    Player player(1);
//    int testSize = 16;

//    player.pushDataToBuff("0000000000000000",testSize);

//    char outputData[] = "1111111111111111";
//    player.readData(outputData,100);
//    QCOMPARE(outputData,"0000000000000000");
}
//QTEST_MAIN(RecorderTest)
