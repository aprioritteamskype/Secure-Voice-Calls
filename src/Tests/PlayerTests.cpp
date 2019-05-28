#include "PlayerTests.h"
#include "player.h"
void PlayerTest::testReadData()
{
    Player player;
    int testSize = 10;
    player.pushDataToBuff("987654321",testSize);

    char outputData[testSize];// place where data shoud be writen
    player.readData(outputData,100);

    QCOMPARE(outputData,"987654321");
}

void PlayerTest::testBufferMaxReadingPos()
{
    Player player(18);
    int testSize = 16;

    player.pushDataToBuff("000000000000000",testSize);
    char outputData[testSize];
    player.readData(outputData,testSize);
    QCOMPARE(outputData,"000000000000000");

    player.pushDataToBuff("111111111111111",testSize);
    player.readData(outputData,testSize);
    QCOMPARE(outputData,"111111111111111");
}

void PlayerTest::testBufferSize()
{
    Player player(100);
    int buffSize = player.getBuffSize();
    QCOMPARE(buffSize,100);
}

void PlayerTest::testBytesToPushOverflow()
{
    Player player(1);
    int testSize = 16;

    player.pushDataToBuff("0000000000000000",testSize);

    char outputData[] = "1111111111111111";
    player.readData(outputData,100);
    QCOMPARE(outputData,"0000000000000000");
}


QTEST_MAIN(PlayerTest)
