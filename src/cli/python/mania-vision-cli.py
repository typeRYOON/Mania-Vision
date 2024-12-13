# mania-vision-cli.py
''' --------------------------------------------------------------------------------------
 >> University of Illinois at Chicago - CS 415, Fall 2024
 >> --------------------------------------------------------------------------------------
 >> File    :: mania-vision-cli.py
 >> Course  :: CS 415 (42844), FA24
 >> Authors :: Ryan Magdaleno
 >>         :: Aaron Willming
 >>         :: Semih Kesler
 >>         :: Jonathan Hung
 >> System  :: Windows 10 w/ Python 3.11.3
 - -            References             - -
 >> https://ai.google.dev/edge/mediapipe/solutions/vision/hand_landmarker
 - -                                   - -
 >> File overview ::
 >> This script takes in a video argument, within that video, a person is playing a
 >> rhythm game on their keyboard. This video will convert the middle + index fingertip
 >> coordinates on both hands to bit encodings for the player presses. Make sure to
 >> install in a venv or global space the requirements.txt libraries before using.
 - -                                   - -
 >> Usage ::
 >> "py mania-vision-cli.py [video-in-path] [encoding-out-path]"
-------------------------------------------------------------------------------------- '''
# Module Imports ::
from typing import List, Tuple, Optional
from fractions import Fraction
import mediapipe as mp
from sys import argv
import numpy as np
import ctypes
import cv2
import os

# Step 0 - Read/Create user config file :: - -                                         - -
def read_config() -> dict:
    settings = {
        "video-in"       : argv[1],
        "encoding-file"  : argv[2],
    }
    if not os.path.exists("config.ini"):
        with open("config.ini", 'w') as f:
            f.write("S D K L 10")
    
    with open("config.ini", 'r') as f:
        config = f.readline().strip().split()
        settings["keys"] = (
            str(ctypes.windll.user32.VkKeyScanW(ord(config[0])) & 0xFF),
            str(ctypes.windll.user32.VkKeyScanW(ord(config[1])) & 0xFF),
            str(ctypes.windll.user32.VkKeyScanW(ord(config[2])) & 0xFF),
            str(ctypes.windll.user32.VkKeyScanW(ord(config[3])) & 0xFF)
        )
        settings["press-threshold"] = int(config[4]) # Shifts pressed y level.

    return settings

# Step 1 - Get each finger's tip coordinate on each frame :: - -                       - -
def collect_finger_points(settings: dict) -> List[List[Tuple[int, int]]]:
    cap    = cv2.VideoCapture(settings.get("video-in"))
    width  = settings["width"]  = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    height = settings["height"] = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
    settings["fps-in"] = cap.get(cv2.CAP_PROP_FPS)

    # Each element is a List of points corresponding to each finger's frame point :
    points: List[List[Optional[Tuple[int, int]]]] = [[], [], [], []]
    last_valid_point: List[List[Tuple[int, int]]] = [[(0,0), (0,0)], [(0,0), (0,0)]]

    with mp.solutions.hands.Hands(
        min_detection_confidence=0.1, min_tracking_confidence=0.85) as mp_hands:

        while cap.isOpened():
            ret, frame = cap.read()
            if not ret: break

            # Process frame via mediapipe hands :
            results = mp_hands.process(cv2.cvtColor(frame, cv2.COLOR_BGR2RGB))

            # R_middle, R_index, L_middle, L_index:
            frame_points: List[List[Tuple[int, int]]] = [[None, None], [None, None]]

            # If at least one hand, get points :
            if results.multi_hand_landmarks and results.multi_handedness:
                # For each hand found, retrieve the middle + index points :
                for hand_i, landmarks in enumerate(results.multi_hand_landmarks):
                    handedness = results.multi_handedness[hand_i].classification[0].label
                    hand = 1 if handedness == "Left" else 0
                    # Middle :
                    last_valid_point[hand][0] = frame_points[hand][0] = (
                        int(landmarks.landmark[12].x * width),
                        int(landmarks.landmark[12].y * height)
                    )
                    # Index :
                    last_valid_point[hand][1] = frame_points[hand][1] = (
                        int(landmarks.landmark[8].x * width),
                        int(landmarks.landmark[8].y * height)
                    )
            # Left middle :
            points[0].append(
                frame_points[0][0] if frame_points[0][0] else last_valid_point[0][0]
            )
            # Left index :
            points[1].append(
                frame_points[0][1] if frame_points[0][1] else last_valid_point[0][1]
            )
            # Right middle :
            points[2].append(
                frame_points[1][0] if frame_points[1][0] else last_valid_point[1][0]
            )
            # Right index :
            points[3].append(
                frame_points[1][1] if frame_points[1][1] else last_valid_point[1][1]
            )
    cap.release()
    return points

# Step 2 - Average out each finger's x/y level to get 4 press coordinates :: - -       - -
def calculate_pressed_points(
        settings: dict,
        points: List[List[Tuple[int, int]]]
    ) -> List[Tuple[int, int]]:

    # Default return :
    pressed_points: List[Tuple[int, int]] = [(0, 0), (0, 0), (0, 0), (0, 0)]

    # Filter edge points out, must be within 1/12 of x and y edges :
    x0, y0 = settings["width"] // 12, settings["height"] // 12
    x1, y1 = settings["width"] - x0,  settings["height"] - y0

    # For each finger :
    for i, finger_point_list in enumerate(points):
        # Filter out edge points :
        valid = [p for p in finger_point_list if p and x0 < p[0] < x1 and y0 < p[1] < y1]

        # Sort by y value and retrieve bottom 75% :
        valid = sorted(valid, key=lambda p: p[1])
        valid = valid[len(valid) // 4:] # Remove outliers

        # Average out remaining points to retrieve this finger's press point :
        pressed_points[i] = (
            int(sum(p[0] for p in valid) / len(valid)),
            int(sum(p[1] for p in valid) / len(valid)) + settings["press-threshold"]
        )

    return pressed_points

# Step 3 - Check on each frame, for each finger, if within press range :: - -          - -
def create_encodings(
    settings: dict,
    points: List[List[Tuple[int, int]]],
    pressed_points: List[Tuple[int, int]]
) -> None:

    encodings = [0] * len(points[0])

    # For each finger :
    for finger_i in range(4):
        press_x = pressed_points[finger_i][0]
        press_y = pressed_points[finger_i][1]

        # For each frame in input video :
        for i in range(len(encodings)):
            # X range :
            if abs(points[finger_i][i][0] - press_x) > 70:
                continue
            # Y range :
            if points[finger_i][i][1] < press_y:
                continue

            # Finger bitmap add :
            encodings[i] |= 2 ** finger_i

    # Create the final output file :
    with open(settings["encoding-file"], "w", encoding="utf-8") as f:
        fps_fraction = Fraction(settings["fps-in"]).limit_denominator(1000000)
        f.write(f"{fps_fraction.numerator} {fps_fraction.denominator} ")
        f.write(f"{' '.join(settings['keys'])}\n")
        for encoding in encodings:
            f.write(f"{encoding}\n")

    return None

# Sequentially process the video and output the mania file :: - -                      - -
def main() -> Optional[str]:
    # Step 0 :: Read config :
    settings = read_config()

    # Step 1 :: Collect points for each finger :
    points = collect_finger_points(settings)

    # Step 2 :: Determine pressed points for each finger :
    pressed_points = calculate_pressed_points(settings, points)

    # Step 3 :: Calculate press encoding for each frame :
    create_encodings(settings, points, pressed_points)

    return None

# Program Entrypoint :: - -                                                            - -
if __name__ == "__main__":
    if len(argv) < 2:
        print("\n > Error :: Missing two arguments \"[video-in] [out-mfile]\"")
    elif len(argv) < 3:
        print("\n > Error :: Missing one argument \"[out-mfile]\"")
    else:
        main()

#   End. :: - -                                                                        - -
