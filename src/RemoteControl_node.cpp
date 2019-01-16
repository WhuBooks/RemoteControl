#include <iostream>
#include "ros/ros.h"
#include "sensor_msgs/Joy.h"
#include "CHTTP_post.h"
#include "CWebSocket.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/thread.hpp>
#include <jsoncpp/json/json.h>
#define steerMax -1.0
using namespace std;

//control
double m_carSpeed = 0;
double m_carWheel = 0;
double m_sendwheel;
double m_speed;

bool flag_sendThread_stop =true;
bool flag_SendThread = false;
bool flag_ReceiveThread =false;
bool flag_Apprequest =false;
bool appfirst=true;
struct JsonStruct
{
    int wheel;
    double speed;
    std::string Id;
    std::string reserve;
};
struct CarStatus
{
    double wheelAngle;
    double vehicleSpeed;
    int gear;
    int controlStatus;
    enum GEAR {
        UNKNOWN=0,
        N = 1,
        D = 2,
        R=3,
        P=4,
        L=5,
        M=6,
    };
    enum controlStatus {
        IDLE = 0,
        START=1,
        STOP=2,
        E_STOP=3,
    };
};
enum CALIBRATE_CONTROL {
    SKIP = -1,
    SAVE = -2,
    CANCEL = -3,

    WHEEL = 0,
    ACCELERATOR = 1,
    BRAKE = 2,
    CLUTCH = 3,

    X_AXIS = 1,
    Y_AXIS = 2,
    Z_AXIS = 3,
    SLIDER_0 = 4,
    SLIDER_1 = 5,
    X_ROT = 6,
    Y_ROT = 7,
    Z_ROT = 8,
};
void callback_control(const sensor_msgs::Joy::ConstPtr& msg){
    //wheel
    double m_wheel = msg->axes[0];
    //cout<<m_sendwheel<<endl;
    //youmen
    double m_youmen =msg->axes[2];
    //brake
    double m_brake =msg->axes[3];

    auto flagg = msg->buttons[0];//stop send
    if(flagg==1)
        flag_SendThread = false;
    auto flagg_stop = msg->buttons[2];//stop send
    if(flagg_stop==1)
    {
        flag_SendThread = true;
        flag_Apprequest = false;
        appfirst=true;
    }

    double a=0;
    int t = 1;//时间差
    //define max deceleration 7.5m/ss;
    if (m_brake > steerMax)//16bit 刹车被踩下
    {
        double delta =fabs( m_brake-steerMax);
        a = delta / (2.0 * steerMax) *10.0;
    }
        //define max accelerated speed 7.5m/ss;
    else if (m_youmen > steerMax)//油门被踩下
    {
        double delta = fabs(steerMax - m_youmen);
        a = -delta / (2.0 * steerMax) *15.0;
    }
    m_speed = m_carSpeed + a * t;
    if (m_speed < 0)m_speed = 0;
    m_sendwheel = double(m_wheel) / double(steerMax) * 520.0;
    //cout<<"111111111111speed:"<<m_speed<<" wheel:"<<m_sendwheel<<endl;
}

void ReceiveThread(){
    flag_ReceiveThread = true;
    kagula::websocket_endpoint endpoint;
    endpoint.connect("ws://192.168.2.133:36795/notify");

    Json::Reader reader;
    Json::Value root;

    while (1) {
        std::vector<std::string> msg;

        endpoint.show(msg);
        //cout<<msg<<endl;
        CarStatus carStatus;
        for (const auto& iter : msg)
        {
            //cout<<iter<<endl;
            if (reader.parse(iter, root))
            {
               // for(const auto& piter:root)
                auto c = root["DevId"].asString();//Mapstatus
                if(c=="RUIHU")
                {
                    carStatus.vehicleSpeed = root["CarStatus"]["vehicleSpeed"].asDouble();
                    carStatus.wheelAngle = root["CarStatus"]["wheelAngle"].asDouble();
                    carStatus.gear = root["CarStatus"]["gear"].asInt();
                    carStatus.controlStatus = root["CarStatus"]["controlStatus"].asInt();
                    if(carStatus.controlStatus>=1)
                    {
                        m_carSpeed = carStatus.vehicleSpeed;
                        m_carWheel = carStatus.wheelAngle;
                    }
                    auto map_s = root["MapStatus"]["map_status"].asInt();
                    if(map_s==5)
                        flag_Apprequest=true;
                }

            }
        }


        //cout<<"car speed:"<<m_carSpeed<<" car wheel:"<<m_carWheel<<endl;
//        if (!flag_ReceiveThread)
//            break;
        usleep(5000);
    }
}

void SendThread(){
    int count =0;
    flag_sendThread_stop = false;
    string url = "http://192.168.2.133:36795/api/AppControl?";//192.168.2.133:36795
    string devID = "devId=RUIHU";
    string action = "&action=RemoteControl";
    url += devID + action;
    //flag_SendThread = true;
    string post;
    string result;
    string errormsg;
    CHTTP_post ht;
    while (1)
    {
        if(flag_Apprequest&&appfirst)
        {
            ROS_INFO("APP request remote control!!!");
            ROS_INFO("APP request remote control!!!");
            appfirst=false;
        }
        auto t1 = ros::Time::now().toSec();
        int w = m_sendwheel;
        double s = m_speed;//to_string(count) ;;//
        //boost::uuids::uuid a_uuid = boost::uuids::random_generator()(); // 这里是两个() ，因为这里是调用的 () 的运算符重载
        const string tmp_uuid = to_string(count) ;//boost::uuids::to_string(a_uuid);
        int re = 0;
        int enable=0;
        if(flag_SendThread)
            enable = 1;
        else
            enable=0;
        post = "{\"wheel\":" + std::to_string(w) + ",\"speed\":" + std::to_string(s) + ",\"Id\":\""+tmp_uuid +"\",\"enable\":"+
               std::to_string(enable)+",\"reserve\":" + std::to_string(re) + "}";

        if(w<530&&w>-530&&s>=0&&s<30)
            ht.xfuture_http_post_json(url, post, result, errormsg);
        else
            ROS_WARN("input invalid...");
        auto t2 = ros::Time::now().toSec();
        auto de = t2-t1;
        //cout<<"send time:"<<de<<endl;
        if(enable)
            cout<<"下发speed:"<<s<<"\t下发wheel:"<<w<<"\t当前speed:"<<m_carSpeed<<"\t当前角度："<<m_carWheel<<endl;
        //usleep(5000);
        if(count>100000)
            count =0;
    }
    flag_sendThread_stop = true;
}
void StartSendThread()
{
    if (!flag_SendThread && flag_sendThread_stop)
    {
        //boost::function0< void> f = boost::bind(&SendThread, NULL);
        boost::thread thread1(&SendThread);
        thread1.detach();
    }
    else
    {
        ROS_WARN("Sending thread is already running!");
        return;
    }
}
void StartReceiveThread()
{
    if (!flag_ReceiveThread)
    {
        //boost::function0< void> f = boost::bind(&CLojiWheelRsvDlg::receiveThread, this);
        boost::thread thread1(&ReceiveThread);
        thread1.detach();
    }
    else
    {
        ROS_WARN("Receive thread is already running!");
        return;
    }
}
int main(int argc,char *argv[]){
    ros::init(argc,argv,"remoteControl");
    ros::NodeHandle n;
    ros::Subscriber joy_sub = n.subscribe("joy",1000,&callback_control);
    StartSendThread();
    StartReceiveThread();
    while (ros::ok())
    {
        ros::spinOnce();
        //cout<<"spinOnce"<<endl;
        usleep(1e3);
    }
    return 0;
}