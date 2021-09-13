#! /usr/bin/ python3

import rospy
from sensor_msgs.msg import Image
from vision_msgs.msg import Detection2D, Detection2DArray
from cv_bridge import CvBridge
import matplotlib.pyplot as plt
import cv2


class PythonNode():
    def __init__(self):
        self.cv_bridge = CvBridge()
        self.image_sub = rospy.Subscriber('/camera/color/image_raw', Image, self.imageCallback, queue_size=1)
        self.detection_sub = rospy.Subscriber('/detectnet/detections', Detection2DArray, self.detectionCallback, queue_size=1)
        self.image_pub = rospy.Publisher('/detected_image', Image, queue_size=1)

        # declare member variables
        self.image = None
        self.detection = None
        self.detection_used = False

        rospy.spin()

    def imageCallback(self, msg):
        self.image = self.cv_bridge.imgmsg_to_cv2(msg, desired_encoding="passthrough")
        im_plotted = self.image.copy()
        if self.detection is not None and not self.detection_used and self.detection.results[0].id == 1:
            c_x = self.detection.bbox.center.x # centre of x
            c_y = self.detection.bbox.center.y # centre of y
            top_left = (int(c_x - self.detection.bbox.size_x / 2), int(c_y - self.detection.bbox.size_y / 2))
            btm_right = (int(c_x + self.detection.bbox.size_x / 2), int(c_y + self.detection.bbox.size_y / 2))
            im_plotted = cv2.rectangle(im_plotted, top_left, btm_right, thickness=2, color=(0,255,0))
            self.detection_used = True
        self.image_pub.publish(self.cv_bridge.cv2_to_imgmsg(im_plotted, encoding="rgb8"))


    def detectionCallback(self, msg):
        self.detection = msg.detections[0]
        self.detection_used = False


if __name__ == '__main__':
    rospy.init_node('python_node')
    try:
        node = PythonNode()
    except rospy.ROSInitException:
        pass
