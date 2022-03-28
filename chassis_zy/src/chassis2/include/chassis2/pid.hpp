#ifndef _PID_HPP_
#define _PID_HPP_



class Pid
{
protected:
    float DEAD_ZONE=0.01;
    float LOC_INTEGRAL_START_ERR=0.03;
    float INTEGRAL_MAX=0.5;
    float V_LIMIT=0.6;
    float T_LIMIT=15.5;

    double error = 0;
    double *V_target = 0;   //
    double *Torque_out = 0; //当前力矩
    double *actual_val = 0;
    double errorlast;
    double integral = 0;
    double Kp = 0.0;
    double Ki = 0;
    double Kd = 0;
    double index_p=1;
public:
    Pid(double *input,double *output,double *target){
        actual_val=input;
        Torque_out=output;
        V_target=target;
    };
    // ~Pid(){};

    int set_target(double vel)
    {
        V_target=&vel;
    }
    float realize(double kp,double ki,double kd ){
        this->Kp=kp;
        this->Ki=ki;
        this->Kd=kd;
        this->error =*V_target - *actual_val;
        if (*V_target==0) {
            *Torque_out=0; 
        return *Torque_out;}
        ROS_INFO("error%.2f, target=%.2f, actuv%.2f",error,*V_target,*actual_val);
//死区设置
	if ((this->error > -this->DEAD_ZONE) && (this->error <this->DEAD_ZONE))
	{
		this->errorlast=0;
		this->error=0;
		this->integral=0;
	}
    
	#if 0
	//积分较大时分离积分项
	if((this->error>-this->LOC_INTEGRAL_START_ERR)&& (this->error<=this->LOC_INTEGRAL_START_ERR))
	this->integral+=this->error;
		//积分范围
		if(this->integral>this->INTEGRAL_MAX)
		{this->integral=this->INTEGRAL_MAX;}
		else if (this->integral<-this->INTEGRAL_MAX)
		{this->integral=-this->INTEGRAL_MAX;}
	#endif
    //启动时，误差较大，输出急剧增大 限制输入
        if (this->error>this->V_LIMIT||this->error<-this->V_LIMIT){
            this->error=this->error/2;
            ROS_INFO("error > V_LIMIT!!!!!!!!!!!!!!!!");
        }
        
    //启动后，速度超过设定速度，err<0,输出依然可能很大。启动后需要力矩较小
        if(this->error<-0.3){index_p=0.6;}else{index_p=1;}
        
        ROS_INFO("kp=%.2f",this->Kp);
        ROS_INFO("index_p=%.2f",index_p);
	//算法实现
	double errout=index_p*this->Kp*this->error+
					this->Ki*this->integral+
					this->Kd*(this->error-this->errorlast);
    if(errout<-T_LIMIT/2) {errout=errout/2;}
ROS_INFO("eout=%.2f",errout);
    *Torque_out+=errout;

    if((*actual_val)>this->V_LIMIT){
        if(this->error>0){
                this->error=0;    }
            
        }
		this->errorlast=this->error;
    if(*Torque_out<0){*Torque_out=-*Torque_out;}
    //输出抗饱和
    if(*Torque_out>T_LIMIT){*Torque_out=T_LIMIT;}
           
ROS_INFO("errorlast=%.2f, tor=%.2f",errorlast,*Torque_out);
	return *Torque_out;
    };

};

#endif