//
//  main.cpp
//  Tests
//
//  Created by Hanzhou Shi on 12/21/15.
//  Copyright © 2015 USF. All rights reserved.
//

#include <iostream>

#include "gtest/gtest.h"

TEST(Sample, Test) {
    EXPECT_EQ(0, 0);
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
