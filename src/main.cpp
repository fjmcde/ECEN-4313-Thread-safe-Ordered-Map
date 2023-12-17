#include "../headers/main.h"

int main(void)
{
    /* TEST 1: Sequential puts and removes */
    printf("TEST 1: Sequential puts and removes");
    map.put(40);
    map.put(5);
    map.put(2);
    map.put(-1);
    map.put(6);
    map.put(4);
    map.put(200);
    map.put(8);
    map.put(1);
    map.remove(1);
    map.put(73);
    map.put(3);
    map.put(7);
    map.put(9);
    map.remove(8);
    map.remove(3);
    map.remove(4);
    map.put(0);
    map.put(20);
    testing::print2DMap();
    map.clear();

    /* TEST 2: Test concurrent inserts */
    printf("\n\nTEST 2: Test concurrent inserts\n");
    testing::fork(testing::threadInsert);
    testing::threadInsert(MAIN_THREAD);
    myThread::join();
    myThread::cleanup();

    testing::print2DMap();

    /* Test 3: Test concurrent getRange */
    printf("\n\nTEST 3: Test concurrent getRange\n");
    testing::fork(testing::threadRange);
    testing::threadRange(MAIN_THREAD);
    myThread::join();
    myThread::cleanup();

    testing::print2DMap();

    /* Test 4: Test concurrent removes */
    printf("\n\nTEST 4: Test concurrent removes\n");
    testing::fork(testing::threadRemove);
    testing::threadRemove(MAIN_THREAD);
    myThread::join();
    myThread::cleanup();

    testing::print2DMap();
    map.clear();

    // Clear worker threads vector
    workers.clear();

    printf("ALL TESTS COMPLETE!");

    return 0;
}
