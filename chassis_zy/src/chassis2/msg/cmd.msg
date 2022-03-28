	# _cmd_str

    int32 control_enable
    int32 control_gear
    int32 control_electric  #  ///按位 0x01左转  0x02右转  0x04 大灯  0x08 制动灯  0x0f  全亮
    int32 control_parking
    float64 control_drive
    float64 control_steering
    float64 control_brake
    float64 control_vel
    float64 kp
    float64 ki
    float64 kd
