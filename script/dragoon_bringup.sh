source ../../../devel/setup.bash
gnome-terminal --tab -- roscore
sleep 1
gnome-terminal --tab -- roslaunch dragoon_bringup dragoon_bringup.launch
sleep 4
source ~/test_ws/devel/setup.bash
gnome-terminal --tab -- roslaunch darknet_ros yolo_v3.launch
sleep 5
gnome-terminal --window -- jtop
