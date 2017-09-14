//
// Created by books on 17-9-9.
//

#include <CommonInclude.h>

int main()
{
    std::string filepath;
    filepath="test.png";
    cv::Mat frame=cv::imread(filepath,cv::IMREAD_COLOR);
    if(frame.empty())
    {
        std::cerr<<"Frame is empty!"<<std::endl;
        return -1;
    }

    cv::Ptr<cv::ORB> orb_det;
    int max_num;
    float scale_factor;
    int level_num;
    int patch_size;
    int score_type;
    int fast_threshold;

    std::cout<<"Input max feature number"<<std::endl;
    std::cin>>max_num;
    std::cout<<"max feature number ~ "<<max_num<<std::endl;

    std::cout<<"Input scale factor"<<std::endl;
    std::cin>>scale_factor;
    std::cout<<"scale factor ~ "<<scale_factor<<std::endl;

    std::cout<<"Input level number"<<std::endl;
    std::cin>>level_num;
    std::cout<<"level number ~ "<<level_num<<std::endl;

    std::cout<<"Input patch size"<<std::endl;
    std::cin>>patch_size;
    std::cout<<"patch size ~ "<<patch_size<<std::endl;

    std::cout<<"Input score type (32~KBytes 0~Harris_Score 1~Fast_Score)"<<std::endl;
    std::cin>>score_type;
    std::string str=score_type==0?"Harris Score":(score_type==1?"Fast Score":"KBytes");
    std::cout<<"score type ~ "<<str<<std::endl;

    std::cout<<"Input fast threshold"<<std::endl;
    std::cin>>fast_threshold;
    std::cout<<"fast threshold ~ "<<fast_threshold<<std::endl;

    std::vector<cv::KeyPoint> vkeypt;
    orb_det=cv::ORB::create(max_num,scale_factor,level_num,patch_size,0,2,score_type,patch_size,fast_threshold);
    timepoint t1=highresclock::now();
    std::string winname=std::to_string(max_num)+"_"+std::to_string(scale_factor)+"_"+std::to_string(level_num)+"_"+str+"_"+std::to_string(fast_threshold);
    orb_det->detect(frame,vkeypt);
    timepoint t2=highresclock::now();
    std::chrono::microseconds span=std::chrono::duration_cast<std::chrono::microseconds>(t2-t1);
    std::cout<<"Orb feature detector spend ~ "<<span.count()<<std::endl;
    cv::Mat draw;
    cv::drawKeypoints(frame,vkeypt,draw);
    cv::imshow(winname,draw);
    cv::waitKey();

}