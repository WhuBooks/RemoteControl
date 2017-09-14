//
// Created by books on 17-9-8.
//

#include <CommonInclude.h>


#define FrameWidth 960
#define FrameHeight 640


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

    //fast feature
    cv::Ptr<cv::FastFeatureDetector> fast_det_ptr;
    std::vector<std::vector<cv::KeyPoint>> vvkeypt;
    int fast_threshold;
    bool fast_nonmaxsupress;
    int fast_type;

    std::cout<<"Please input fast threshold : "<<std::endl;
    std::cin>>fast_threshold;
    std::cout<<"fast threshold ~ "<<fast_threshold<<std::endl;

    std::cout<<"Please input fast use non-max suppress (1~true 0~false) : "<<std::endl;
    int val;
    std::cin>>val;
    fast_nonmaxsupress= val != 0;
    std::cout<<"fast use non-max suppress ~ "<<fast_nonmaxsupress<<std::endl;

    std::cout<<"Please input fast type (0~Type_5_8 1~Type_7_12 2~Type_9_16)"<<std::endl;
    std::cin>>val;
    std::string strtype=val==0?"Type_5_8":(val==1?"Type_7_12":(val==2?"Type_9_16":"unknown"));
    std::cout<<"fast type ~ "<<strtype<<std::endl;

    timepoint t1=highresclock::now();
    std::vector<cv::Mat> vframe;
    vframe.push_back(frame);
    for(int i=1;i<5;i++)
    {
        cv::Mat temp;
        cv::resize(frame,temp,cv::Size(frame.cols/(i*2),frame.rows/(i*2)));
        vframe.push_back(temp);
    }
    fast_det_ptr=cv::FastFeatureDetector::create(fast_threshold,fast_nonmaxsupress,fast_type);
    fast_det_ptr->detect(vframe,vvkeypt);
    timepoint t2=highresclock::now();
    std::chrono::microseconds span=std::chrono::duration_cast<std::chrono::microseconds>(t2-t1);
    std::cout<<"Detect spend time ~ "<<span.count()<<std::endl;

    for(std::size_t i=0;i<vframe.size();i++)
    {
        std::string winname=std::to_string(fast_threshold)+"_"+std::to_string(fast_nonmaxsupress)+"_"+std::to_string(fast_type)+"_"+std::to_string(i);
        cv::Mat temp_frame=vframe[i];
        std::vector<cv::KeyPoint> temp_vkeypt=vvkeypt[i];
        cv::Mat temp_draw;
        cv::drawKeypoints(temp_frame,temp_vkeypt,temp_draw);
        cv::imshow(winname,temp_draw);
        std::cout<<winname<<" ~ detect ~"<<temp_vkeypt.size()<<std::endl;
    }
    cv::waitKey(0);
    cv::destroyAllWindows();
}