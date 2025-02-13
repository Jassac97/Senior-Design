import serial
import time
import inference
import cv2 as cv
import math
import chess
from chess import engine
import numpy as np
import stockfish
from stockfish.models import Stockfish
from image_processing import resize_image, binarize_image, auto_canny, hough_line, intersections, cluster, find_corners, get_image_from_four_points, hor_vert_lines
from chess_engine import board_to_fen, move_to_space, is_next_position_occupied, manipulator_3DOF

model = inference.get_model("chesspieces-eswj2/3")
board_map = np.array([
    [[-9.625, 1.375], [-6.875, 1.375], [-4.125, 1.375], [-1.375, 1.375], [1.375, 1.375], [4.125, 1.375], [6.875, 1.375], [9.625, 1.375]],
    [[-9.625, 4.125], [-6.875, 4.125], [-4.125, 4.125], [-1.375, 4.125], [1.375, 4.125], [4.125, 4.125], [6.875, 4.125], [9.625, 4.125]],
    [[-9.625, 6.875], [-6.875, 6.875], [-4.125, 6.875], [-1.375, 6.875], [1.375, 6.875], [4.125, 6.875], [6.875, 6.875], [9.625, 6.875]],
    [[-9.625, 9.625], [-6.875, 9.625], [-4.125, 9.625], [-1.375, 9.625], [1.375, 9.625], [4.125, 9.625], [6.875, 9.625], [9.625, 9.625]],
    [[-9.625, 12.375], [-6.875, 12.375], [-4.125, 12.375], [-1.375, 12.375], [1.375, 12.375], [4.125, 12.375], [6.875, 12.375], [9.625, 12.375]],
    [[-9.625, 15.125], [-6.875, 15.125], [-4.125, 15.125], [-1.375, 15.125], [1.375, 15.125], [4.125, 15.125], [6.875, 15.125], [9.625, 15.125]],
    [[-9.625, 17.875], [-6.875, 17.875], [-4.125, 17.875], [-1.375, 17.875], [1.375, 17.875], [4.125, 17.875], [6.875, 17.875], [9.625, 17.875]],
    [[-9.625, 20.625], [-6.875, 20.625], [-4.125, 20.625], [-1.375, 20.625], [1.375, 20.625], [4.125, 20.625], [6.875, 20.625], [9.625, 20.625]]
])

MAX_WIDTH = 1000
DESIRED_WIDTH = 2000
# Set up serial connection (change 'COM3' to your port)
arduino = serial.Serial('COM3', 9600, timeout=1)

time.sleep(2)

while True:
    robot_turn = arduino.readline().decode().strip()  # Read and decode data
    print(robot_turn)
    if robot_turn == 1:
        ### OpenCV: Start
        cam = cv.VideoCapture(1, cv.CAP_DSHOW)
        cam.set(cv.CAP_PROP_FRAME_WIDTH, 4000)
        cam.set(cv.CAP_PROP_FRAME_HEIGHT, 2160)
        if not cam.isOpened():
            print("Failed to open camera")
        else:
            result, original = cam.read()
            if result:
                img = resize_image(original)    
                H,W,_ = img.shape
                BW = binarize_image(img) # Turn image into BW (black white image)
                edge = auto_canny(BW) # Use Canny technique to get the edges of image
                lines = hough_line(edge ) # Use Houghline to detech lines and plot image
                h,v = hor_vert_lines(lines)  # Find the intersections of horizon and vertical lines, cluster them and display on the image:
                points = intersections(h,v)
                clustered_points = cluster(points, max_dist=20)
                corners = find_corners(clustered_points,H,W) # Find corners:
                warp = get_image_from_four_points(img, corners) # Get the board
        ### OpenCV: Done

        ### Chess Engine: Start
        height, width,_ = warp.shape
        num_rows = 8
        num_cols = 8
        square_width = width // num_cols
        square_height = height // num_rows
        board = np.empty((num_rows, num_cols), dtype=object)
        response = model.infer(warp)
        
        for prediction in response[0].predictions: # Get prediction
            x, y, w, h = int(prediction.x), int(prediction.y), int(prediction.width), int(prediction.height)
            label = prediction.class_name

            xmin = int(x - w/2)
            xmax = int(x + w/2)
            ymin = int(y - h/2)
            ymax = int(y + h/2)

            max_overlap = 0
            best_square = None
            
            for row in range(num_rows):
                for col in range(num_cols):
                    # Define the square (top-left and bottom-right corners)
                    square_x_min = col * square_width
                    square_y_min = row * square_height
                    square_x_max = (col + 1) * square_width
                    square_y_max = (row + 1) * square_height

                    # Calculate the intersection area
                    overlap = intersection_area([xmin, ymin, xmax, ymax], [square_x_min, square_y_min, square_x_max, square_y_max])
                    
                    if overlap > max_overlap:
                        max_overlap = overlap
                        best_square = (row, col)
            if best_square:
                board[best_square] = label

        fen = board_to_fen(board)
        # Set up chess engine Stockfish
        board_chess = chess.Board()
        stockfish = Stockfish(r'C:\Users\tttha\Documents\Fall_2024\EE175\stockfish-windows-x86-64-avx2\stockfish\stockfish-windows-x86-64-avx2.exe')
        stockfish.set_depth(20)
        stockfish.set_skill_level(15)
        stockfish.get_parameters()

        board_chess.set_fen(fen)
        stockfish.set_fen_position(board_chess.fen())
        best_move = stockfish.get_best_move()
        board_chess.push_san(best_move)
        coordinates = move_to_space(best_move,board_map)
        angles = []
        for coordinate in coordinates:
            angles.append(manipulator_3DOF(L1, L2, L3,coordinate,-100*np.pi/180))
        
        data_str = ";".join([",".join(map(str, sublist)) for sublist in angles])
        # Send data to Arduino
        arduino.write(f"{data_str}\n".encode()) 


