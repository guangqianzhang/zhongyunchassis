	# _cmd_str

    uint8 car_state
    uint8 Auto_Enb
    uint8 Gear_Enb_fbk  #  ///按位 0x01左转  0x02右转  0x04 大灯  0x08 制动灯  0x0f  全亮
    uint8 Steering_Enb_fbk
    uint8 Drive_Enb_fbk
    uint8 Brake_Enb_fbk
    uint8 Parking_Enb_fbk
    uint8 RC_Takeover_flg
    
# uint8 err_Code
# uint8 Code_Num

# uint8 soc_
# uint8 Bat_Discharge_Cur
# float64 Bat_Vol
    # int32 err_Code; # //故障码
	# int32 Code_Num;  #	//具体故障码
    # int32 battery;			   #// soc
	# int32 Bat_Discharge_Cur;  #//放电电流


