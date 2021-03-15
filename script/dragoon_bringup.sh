gnome-terminal --tab -- roscore
sleep 3
gnome-terminal --tab -- roslaunch dragoon_bringup dragoon_bringup.launch
sleep 4
gnome-terminal --tab -- roslaunch darknet_ros yolo_v3.launch
sleep 5
gnome-terminal --window -- jtop
