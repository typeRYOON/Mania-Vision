import cv2
import mediapipe as mp
import numpy as np

# Reference for angle calculation,
# "Calculating Hand Kinematic" https://www.youtube.com/watch?v=EgjwKM3KzGU

# Initializes Video and Gets Video Info
videoPath = "forward.mov"
capture = cv2.VideoCapture(videoPath)

if not capture.isOpened():
    print("Error: file not opened")
    exit(1)

file = open("test.txt","w")

fps = capture.get(cv2.CAP_PROP_FPS)
videoWidth = int(capture.get(cv2.CAP_PROP_FRAME_WIDTH))
videoHeight = int(capture.get(cv2.CAP_PROP_FRAME_HEIGHT))

# Initializes Media Pipe Hand Detection
mpHands = mp.solutions.hands
hands = mpHands.Hands(static_image_mode=False,max_num_hands=1, min_detection_confidence=0.5,min_tracking_confidence=0.5)
mpDraw = mp.solutions.drawing_utils

# Specifies Joints
# 0 Index, 1 Middle, 2 Ring, 3 Pinky
Joints = [[5,6,8],[9,10,12],[13,14,16],[17,18,20]]

# Find angle between joints
def fingerAngle(a,b,c):
    aPos = np.array((a.x,a.y))
    bPos = np.array((b.x,b.y))
    cPos = np.array((c.x,c.y))

    radians = np.arctan2(cPos[1]-bPos[1],cPos[0]-bPos[0]) - np.arctan2(aPos[1]-bPos[1],aPos[0]-bPos[0])
    angle = np.abs(radians*180.0/np.pi)

    if angle > 180.0:
        angle = 360-angle

    return angle

def binaryJoints(landmarks):
    out = 0
    for joint in Joints:
        jointAngle = fingerAngle(landmarks[joint[0]],landmarks[joint[1]],landmarks[joint[2]])
        if jointAngle < 160:
            out += 2 ** (abs(Joints.index(joint)-3))
    file.write(str(out))
    file.write("\n")

while capture.isOpened():
    success, frame = capture.read()
    if not success:
        break

    grayFrame = cv2.cvtColor(frame,cv2.COLOR_BGR2RGB)
    results = hands.process(grayFrame)

    if results.multi_hand_landmarks:
        # for hand in results.multi_hand_landmarks:
        firstLandmark = results.multi_hand_landmarks[0]
        mpDraw.draw_landmarks(frame,firstLandmark,mpHands.HAND_CONNECTIONS)

        handLandmarks = firstLandmark.landmark
        binaryJoints(handLandmarks)

file.close()






