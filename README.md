# Our beloved dragoon
password: h0wd3!

Important installation:

## Darknet ros
https://github.com/leggedrobotics/darknet_ros/issues/290

https://github.com/ros-perception/vision_opencv/pull/259
https://github.com/fizyr-forks/vision_opencv/tree/opencv4

## Crude steps for reinstallation of Dragoons brains
Hi folks, my name is Dan bronstein and I am a fucking dumbass; I uninstalled our entire ROS system. Whoops.

What's that....? You are too? Welcome to the shitshow idiot. Here are the steps I took to recover from this mess. I am reciting this from memory because I'm not going to uninstall everything again just to make sure these instructions are accurate. I am not liable for destroying your robot, that's your fault.

At a high level this will handle installing ROS melodic, stuff to get realsense working with ROS, and tensorRT. Again, I'm writing this from memory so if it doesn't work for you I don't care and it's not my fault.

## Reinstall ROS

No shit

```bash
sudo apt install ros-melodic-desktop-full
```

## Install that massive list of files

After I fucked everything up and accidently uninstalled our system, I went into the `apt` log files in `var/log/apt/history.log` to see what it just uninstalled. And damn there was a lot. Like, way more than I was going to copy paste by hand. So save yourself some time and use regex to capture all the names of the uninstalled packages. `grep` sucks at capture groups so I recommend `pcregrep` or `awk` or whatever. Don't know how regex capture groups work? Too bad. 

Anyways, I'll assume you have a file that lists all the packages you just uninstalled, each a single line of a text file called `fuckinghell.txt`. Okay now reinstall it.

```
xargs sudo apt-get -y install < fuckinghell.txt
```

Try and run your system. Did it work? If yes then good, don't tell your team that you fucked everything up. But I know what you did. I will always know.

Did it not work? FUCK. Let me guess, realsenseROS and realsenselib2 are complaining? Welcome to realsense hell.

## Realsense is garbage

Cool so remember theres like 45 things neccessary to get realsense running in ROS. Just kidding, its 2: `librealsense2` and `realsense-ros`. Both have packages available through `apt-get` so this should be easy right? Why is it always fucking up? 

Welp here's what happened for us: We had locally installed versions of `librealsense` that were different than the most recent repo from `apt-get` install. Different versions of `realsense-ros` require specific different versions of `librealsense2`. If you have a local installation of `librealsense2` installed sneakily on your system, it's gonna throw a wrench in everything. Turns out thats what we had: `librealsense2-v2.4.4` was sneakily living installed on our system, while the apt-installs were repeatedly bring on `librealsense-v2.49` and the *its* compatible `realsense-ros` version. However, `realsense-ros` would then get built using the sneaky `v2.44` and everything broke. 

```
sudo apt install mlocate
mlocate librealsense2
```

Do you see a bunch of things like `librealsense2.44.so*` or whatever showing in `/usr/lib` or other important system directories? Then you probabyly also have a sneaky install of `librealsense` on your system. Great. I guess ne option is to uninstall that and reinstall the librealsense stuff from apt-get. I didn't do that. I did:

### Tear it all up

Idk which of these are neccessary
```
sudo apt-get remove librealsense2-dkms
sudo apt-get install librealsense2-utils
sudo apt-get install librealsense2-dev
sudo apt-get install librealsense2-dbg
```

If after that, you can still launch `realsense-viewer` from the command line, then you definitely have a sneaky install living on your system.. find the version of that sneaky install using the `locate` command above. Go to Intel's [realsense-ros release page](https://github.com/IntelRealSense/realsense-ros/tags) and click around until you find the verison compatible with `librealsense` installed on your system. Follow the [Jetson installation procedures](https://github.com/IntelRealSense/librealsense/blob/master/doc/installation_jetson.md), but make sure to clone the correct version, don't just clone master!

The build it and hopefully realsense works now. If not that sucks for you

## Getting tensorRT to work with ROS melodic

We use [this library](https://github.com/indra4837/yolov4_trt_ros) to depoloy darknet models in the tensorRT runtime env in ROS. This lib has some stuff written in python3. **DO NOT ATTEMPT TO INSTALL ros-melodic-python3-pkg**. This will, allegedly, unintsall all your ROS stuff. Who knew.

Anyways, follow the instructions for install stuff. It's fine that onnx and pycuda get installed in python3 because the conversion step from darknet->onnx->`.trt` happends ourside of the ROS environment. 

Alright great now you can convert your models to `.trt`, however you can't run them in ROS yet, because `trt_yolo_v4.py` will complain about not having the right things installed. This is the file that is launched from the ROS launch node, so it needs to be python2 compatible. go ahead and change the shebang line to `#!/usr/bin/env python2` to force python2 execution. Now it will complain you don't have pycuda. Okay smarty pants, guess what?

(I forget what the comman was tbh, but this makes sense)

```
pip2 install pycuda==2019.1.2
```

Okay now just fix the general syntax errors in `trt_yolo_v4.py` and the fact that it reads in launch params incorrectly, and you are good to go. Maybe. Idk. Good luck
