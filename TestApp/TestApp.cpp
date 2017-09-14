//
// Created by books on 17-9-7.
//

#include "TestLib.h"
#include <unsupported/Eigen/MatrixFunctions>

int main()
{
    TestLib t;
    Eigen::Vector3d vec_rot;
    vec_rot<<1,1,1;
    std::cout<<vec_rot<<std::endl;
    Sophus::SO3 so3=Sophus::SO3::exp(vec_rot);
    Eigen::Matrix3d so3_mat=so3.matrix();
    std::cout<<so3_mat<<std::endl;
    t.Hello();
    return 0;
}