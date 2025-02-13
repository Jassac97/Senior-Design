def board_to_fen(board):
    # Define the piece mappings
    piece_map = {
        'rook-white': 'R', 'knight-white': 'N', 'bishop-white': 'B', 
        'queen-white': 'Q', 'king-white': 'K', 'pawn-white': 'P',
        'rook-black': 'r', 'knight-black': 'n', 'bishop-black': 'b', 
        'queen-black': 'q', 'king-black': 'k', 'pawn-black': 'p',
        None: ''
    }
    
    fen_rows = []
    
    for row in board:
        fen_row = ''
        empty_count = 0
        
        for square in row:
            piece = piece_map[square]
            if piece:  # If there's a piece
                if empty_count > 0:
                    fen_row += str(empty_count)  # Add number of empty squares
                    empty_count = 0
                fen_row += piece
            else:  # If it's an empty square
                empty_count += 1
        
        if empty_count > 0:  # If the row ends with empty squares
            fen_row += str(empty_count)
        
        fen_rows.append(fen_row)
    
    return '/'.join(fen_rows)
def move_to_space(move, board_square_positions, z=2):
    """
    Convert a chess move into initial and target physical space coordinates.
    
    Args:
        move (str): A string representing the move, e.g., "e2e4".
        board_map (dict): A dictionary mapping chessboard positions to physical coordinates.
        z (float): The Z-coordinate for t he movement, default is 0.
        
    Returns:
        list: A list of two positions [[x_init, y_init, z], [x_next, y_next, z]].
    """
    init_column_index = ord(move[0]) - ord('a') + 1
    init_row_index = int(move[1])
    
    next_column_index = ord(move[2]) - ord('a') + 1
    next_row_index = int(move[3])
    x_init, y_init = board_map[init_row_index-1,init_column_index-1]

    x_next, y_next = board_map[next_row_index-1,next_column_index-1]
    pos = [[x_init,y_init,z],[x_next,y_next,z]]

    return pos
def is_next_position_occupied(board,move):
    # Check the next location
    next_column_index = ord(move[2]) - ord('a') + 1
    next_row_index = int(move[3])
    print(np.flip(board)[next_row_index-1,next_column_index-1])
    if (np.flip(board)[next_row_index-1,next_column_index-1] != None):
        return True
    else:
        return 
def manipulator_3DOF(L1,L2,L3, pos, end_angle):
    x, y, z = pos
    # Compute theta4
    theta4 = np.arctan2(y,x)
    
    r = np.sqrt(x**2 + y**2 + z**2)
    if r > L1 + L2 + L3:
        raise ValueError("Target is unreachable")
        
    new_y = z - 9.4
    new_x = np.sqrt(x**2 + y**2)
 
    if np.abs(end_angle) > np.pi/2:
        p2x = new_x + L3 * np.sin(np.abs(end_angle) - np.pi/2)
        p2y = new_y + L3 * np.cos(np.abs(end_angle) - np.pi/2)
        print(p2x, p2y)
    else:
        p2x = new_x - L3 * np.cos(end_angle)
        p2y = new_y - L3 * np.sin(end_angle) 
        print(p2x, p2y)
    
    r2 = np.sqrt(p2x**2 + p2y**2)
    print(r2)
    if r2 > L1 + L2:
        raise ValueError("End effector angle is not achievable")

    # Compute theta2
    c2 = (r2**2 - L1**2 - L2**2) / (2 * L1 * L2)
    s2 = -np.sqrt(1-c2**2)
    theta2 = np.arctan2(s2,c2)

    # Compute theta1
    s1 = (p2y*(L1 + L2*c2) - L2*s2*p2x)/(L1**2 + L2**2 + 2*L1*L2*c2)
    c1 = (p2x*(L1 + L2*c2) + L2*s2*p2y)/(L1**2 + L2**2 + 2*L1*L2*c2)
    theta1 = np.arctan2(s1,c1)

    # Compute theta3
    theta3 = end_angle - theta1 - theta2
    return [theta1, theta2, theta3, theta4]