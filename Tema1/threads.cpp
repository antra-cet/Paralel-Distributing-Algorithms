#include <iostream>
#include "threadsUtils.h"

using namespace std;

int main(int argc, char *argv[]) {
    utils_t utils;
    init(argc, argv, utils);

    threadCreate(utils);

    // for (int i = 0; i < utils.mapperThreads; i++) {
    //     for (int j = 2; j <= utils.reducerThreads; j++) {
    //         for (auto it = utils.exponents.at(i)[j].begin(); it != utils.exponents.at(i)[j].end(); ++it) {
    //             cout << *it << " ";
    //         }
    //     }

    //     cout << endl;
    // }

    // TODO : merge the elements, reducer
    threadJoin(utils);

    // threadsPrint();

    threadExit(utils);
}
