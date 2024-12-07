# GUI is WIP do not use right now.
from typing import List, Tuple, Optional
from fractions import Fraction
import mediapipe as mp
from sys import argv
import numpy as np
import ctypes
import cv2
import os

#
def read_config() -> dict:
    settings = {
        "video-in"       : f"mania-files/input-videos/{argv[1]}",
        "video-out"      : f"mania-files/output-videos/{argv[2]}.mp4",
        "encoding-file"  : f"mania-files/__mania__/{argv[2]}.txt",
    }
    if not os.path.exists("mania-files/config.ini"):
        with open("mania-files/config.ini", 'w') as f:
            f.write("S D K L 10 20")
    
    with open("mania-files/config.ini", 'r') as f:
        config = f.readline().strip().split()
        settings["keys"] = (
            str(ctypes.windll.user32.VkKeyScanW(ord(config[0])) & 0xFF),
            str(ctypes.windll.user32.VkKeyScanW(ord(config[1])) & 0xFF),
            str(ctypes.windll.user32.VkKeyScanW(ord(config[2])) & 0xFF),
            str(ctypes.windll.user32.VkKeyScanW(ord(config[3])) & 0xFF)
        )
        settings["y-upshift"]       = int(config[4])
        settings["press-threshold"] = int(config[5])

    return settings

#
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

            # L_middle, L_index, R_middle, R_index :
            frame_points: List[List[Optional[Tuple[int, int]]]] = [[None, None], [None, None]]

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

# 
def calculate_pressed_points(
        settings: dict,
        points: List[List[Tuple[int, int]]]
    ) -> List[Tuple[int, int]]:

    # Default return :
    pressed_points: List[Tuple[int, int]] = [(0, 0), (0, 0), (0, 0), (0, 0)]

    # Filter edge points out, must be within 1/12 of x and y edges :
    x0, y0 = settings["width"] // 12, settings["height"] // 12
    x1, y1 = settings["width"] - x0,  settings["height"] - y0

    #img = np.ones((1920, 1920, 3), dtype=np.uint8)
    for i, finger_point_list in enumerate(points):
        valid = [p for p in finger_point_list if p and x0 < p[0] < x1 and y0 < p[1] < y1]

        # Sort by y value and average out bottom 75%
        valid = sorted(valid, key=lambda p: p[1])
        valid = valid[len(valid) // 4:] # Remove highest outliers

        pressed_points[i] = (
            int(sum(p[0] for p in valid) / len(valid)),
            int(sum(p[1] for p in valid) / len(valid)) + settings["y-upshift"]
        )

    return pressed_points

#
def create_encodings(
    settings: dict,
    points: List[List[Tuple[int, int]]],
    pressed_points: List[Tuple[int, int]]
) -> Tuple[List[int], int]:

    threshold = settings["press-threshold"]
    encodings = [0] * len(points[0])

    for finger_i in range(4):
        press_x = pressed_points[finger_i][0]
        press_y = pressed_points[finger_i][1] - threshold

        for i in range(len(encodings)):
            if abs(points[finger_i][i][0] - press_x) > 70:
                continue
            if points[finger_i][i][1] < press_y:
                continue
            encodings[i] |= 2 ** finger_i

    #
    averaged_kps_graph = list()
    step = len(encodings) // 31
    for i in range(0, len(encodings), step):
        averaged_kps_graph.append(
            str(sum(encodings[i:i+min(step, len(encodings)-i)]) // step)
        )

    with open(settings["encoding-file"], "w", encoding="utf-8") as f:
        fps_fraction = Fraction(settings["fps-in"]).limit_denominator(1000000)
        f.write(f"{fps_fraction.numerator} {fps_fraction.denominator} ")
        f.write(f"{' '.join(settings['keys'])}\n")
        f.write(f"{chr(10).join(averaged_kps_graph[:30])}\n!\n")
        for encoding in encodings:
            f.write(f"{encoding}\n")

    # Find the earliest press :
    for i in range(len(encodings)):
        if 0b1111 & encodings[i]:
            return encodings, i

    return encodings, 0 # No press found at all.

# generate_output_video(settings, points, pressed_points, encodings)
def generate_output_video(
    settings: dict,
    points: List[List[Tuple[int, int]]],
    pressed_points: List[Tuple[int]],
    encodings: List[int],
    start_frame: int
) -> None:

    cap        = cv2.VideoCapture(settings["video-in"])
    fps_in     = int(cap.get(cv2.CAP_PROP_FPS))
    frame_skip = max(1, round(fps_in / 24))

    out = cv2.VideoWriter(
        settings["video-out"],
        cv2.VideoWriter_fourcc(*"mp4v"),
        24,
        (settings["width"], settings["height"])
    )

    # Trim to first press :
    cap.set(cv2.CAP_PROP_POS_FRAMES, start_frame)
    encodings = encodings[start_frame:]
    for i in range(4):
        points[i] = points[i][start_frame:]

    #
    threshold = settings["press-threshold"]
    
    l: List[Tuple[int, int]] = list()
    for i in range(4):
        l.append(int(pressed_points[i][0] - 70))
        l.append(pressed_points[i][1] - threshold)
        l.append(int(pressed_points[i][0] + 70))
        l.append(pressed_points[i][1] - threshold)

    frame_i = -1
    while cap.isOpened():
        ret, frame = cap.read()
        if not ret: break

        frame_i += 1
        if frame_i % frame_skip == 0:
            continue

        for finger_i in range(4):
            color = (0, 0, 255) if encodings[frame_i] & 2 ** finger_i else (255, 0, 0)
            cv2.circle(frame,
                (int(points[finger_i][frame_i][0]), int(points[finger_i][frame_i][1])),
                8, color, -1 )
            cv2.line(
                frame,
                (l[finger_i*4],     l[finger_i*4 + 1]),
                (l[finger_i*4 + 2], l[finger_i*4 + 3]),
                color,
                2
            )

        out.write(frame)

    cap.release()
    out.release()

    return None

#
def main() -> Optional[str]:
    # Step 0 :: Read config :
    settings = read_config()

    # Step 1 :: Collect points for each finger :
    points = collect_finger_points(settings)

    # Step 2 :: Determine pressed points for each finger :
    pressed_points = calculate_pressed_points(settings, points)

    # Step 3 :: Calculate press encoding for each frame :
    encodings = create_encodings(settings, points, pressed_points)

    # Step 4 :: Generate output video :
    generate_output_video(settings, points, pressed_points, *encodings)

    return None

#
if __name__ == "__main__":
    main()
