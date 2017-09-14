//
// Created by books on 17-9-7.
//
#ifndef VOLIB_TESTLIB_H
#define VOLIB_TESTLIB_H
#include <iostream>
#include <sophus/se3.h>
#include <sophus/so3.h>
#include <Eigen/Core>

class TestLib {
public:
    TestLib(){};
    void Hello();

    Sophus::SE3 se3;
    Eigen::Matrix3d R;
    Eigen::Vector3d t;
};


#endif //VOLIB_TESTLIB_H
