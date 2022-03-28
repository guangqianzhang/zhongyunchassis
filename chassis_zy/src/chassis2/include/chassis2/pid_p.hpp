#ifndef _PID_HPP_
#define _PID_HPP_
#include <iostream>
#include <cmath>

class Pid
{
protected:
    float DEAD_ZONE = 0.01;
    float ERR_LIMIT=0.5;
    float ERR_LIMIT2=0.3;
    float V_LIMIT = 0.6;
    float T_LIMIT = 14;
    float STRONG=1.3;
    float WEAK=0.5;
    float NORMAL=1;

    double error = 0;
    double *V_target = 0;   //
    double *Torque_out = 0; //当前力矩
    double *actual_val = 0;
    double errorlast=0.0;
    double errorPrev=0.0;
    double err_c = 0;
    double err_cc=0;
    float Kp = 0.0;
    float Ki = 0;
    float Kd = 0;

    float index=1;
    float index_p=1;
    float index_i=0;
public:
    Pid(double *input, double *output, double *target)
    {
        actual_val = input;
        Torque_out = output;
        V_target = target;
    };
    // ~Pid(){};

    int set_target(double vel)
    {
        V_target = &vel;
    }
    float realize(float kp, float ki, float kd)
    {
        this->Kp = kp;
        this->Ki = ki;
        this->Kd = kd;
        this->error = *V_target - *actual_val;
        this->err_c=this->error - this->errorlast;
        this->err_cc=this->errorlast - this->errorPrev;
        if (*V_target == 0)
            return 0;
        ROS_INFO("error%.2f, target=%.2f, actuv%.2f", error, *V_target, *actual_val);

//算法实现
        
        //启动时，误差较大，输出急剧增大 限制输入

        if(abs(this->error)>this->ERR_LIMIT){
            if(this->error<0){
                index=WEAK;
            }else{index=STRONG;}
            index_p=STRONG;
            index_i=0;
            ROS_INFO("1111");
        }
        if((this->err_c>0 && this->error >0)
        || (this->err_c<0 && this->error <0)){
            ROS_INFO("22222");
            //err_c*err_c > 0 误差增大
            if (abs(this->error)>=this->ERR_LIMIT2){
                index=NORMAL;
                index_p=STRONG;
                index_i=0;
            }else if (abs(this->error) < this->ERR_LIMIT2){
                index=NORMAL;
            }
            if(this->err_c=0){
                index_i=1;
                }
        }else if(this->error|this->err_c){
                //err_c*err_c < 0 误差减小
                ROS_INFO("33333");
            if((this->err_c>0 && this->err_cc <0)
              ||(this->err_c<0 && this->err_cc >0)){
                index=NORMAL;
                ROS_INFO("44444");
            }else if(this->err_c|this->err_cc){
                //极限
                ROS_INFO("5555");
                if(abs(this->error)>this->ERR_LIMIT2){
                    index_p=STRONG;
                    index_i=STRONG;
                }else{
                    ROS_INFO("66666");
                    index_p=WEAK;
                    index_i=WEAK;
                }
            }
        }
        if(abs(this->error)<this->DEAD_ZONE){
            ROS_INFO("77777");
                this->error=0;
                this->err_c=0;
                this->err_cc=0;
        }


        double errout = index_p*this->Kp * (this->error -this->errorlast)+
                        index_i*this->Ki * this->error +
                        this->Kd * (this->error - 2*this->errorlast+ this->errorPrev);
    //启动后，速度超过设定速度，err<0,输出依然可能很大。启动后需要力矩较小
       //防止输出急剧减小，小车停止
        if (errout < -T_LIMIT / 2)
        {
            errout = errout / 2;
        }

        *Torque_out +=index * errout;

        //规范输出
        if (*Torque_out < 0)
        {
            *Torque_out = -*Torque_out;
        }
        //输出抗饱和
        if((*actual_val)>this->V_LIMIT){
            if(this->error>0){
                this->error=0;;
            }else if (this->error<0){

            }
            *Torque_out =*Torque_out /2;
        }
        if (*Torque_out > T_LIMIT)
        {
            *Torque_out = T_LIMIT;
        }
        //误差更新
        this->errorPrev = this->errorlast;
        this->errorlast=this->error;
        ROS_INFO("pre err%.2f, last err%.2f",this->errorPrev,this->errorlast);
        ROS_INFO("out%.2f,tor%.2f", errout, *Torque_out);
        return *Torque_out;
    };
};

#endif
