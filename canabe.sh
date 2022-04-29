ifconfig can0 down
sudo ip link set can0 type can bitrate 500000 restart-ms 100
ifconfig can0 up
ip -details link show can0
