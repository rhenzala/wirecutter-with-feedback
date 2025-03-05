import cv2
import numpy as np

# Replace "your_camera_stream_url" with the actual URL of your ESP32-CAM video stream
cap = cv2.VideoCapture("http://192.168.43.102:81/stream")

while True:
    ret, frame = cap.read()  # Read a frame from the video stream

    # Convert the frame to grayscale
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # Apply edge detection to find the edges
    edges = cv2.Canny(gray, 50, 150)
    #kernel = np.ones((5, 5))
    #img_dilate = cv2.dilate(edges, kernel, iterations=3)
    #img_thresh = cv2.erode(img_dilate, kernel, iterations=2)

    middle = edges.shape[1] // 2
    lines = cv2.HoughLinesP(edges, 1, np.pi /1000, 50, minLineLength=middle, maxLineGap=50)

    # Check if any lines were detected
    if lines is not None:
        # Calculate the length of each detected line
        line_lengths = [np.sqrt((line[0][2] - line[0][0]) ** 2 + (line[0][3] - line[0][1]) ** 2) for line in lines]
        # Find the index of the line closest to the middle of the image
        middle_line_index = np.argmin([abs((line[0][0] + line[0][2]) / 2 - middle) for line in lines])
        # Get the coordinates of the middle line
        x1, y1, x2, y2 = lines[middle_line_index][0]
        #print(x1, y1, x2, y2)
        # Draw the middle line on the image
        cv2.line(frame, (x1, y1), (x2, y2), (0, 0, 255), 3)
        # Find the index of the longest line
        longest_line_index = np.argmax(line_lengths)
        # Display the length of the longest line
        length_pixels = line_lengths[longest_line_index]
        pixels_per_cm_x = edges.shape[1] / 15
        pixels_per_cm_y = edges.shape[0] / 5
        length_cm = (length_pixels / (0.5 * (pixels_per_cm_x + pixels_per_cm_y)))
        cv2.putText(frame, '{:.2f} cm'.format(length_cm), (x1 + 20, y1 - 20), cv2.FONT_HERSHEY_SIMPLEX, 1.0,
                    (255, 0, 255), 3)
        print(length_cm)

        # Display the frame with detected lines
        cv2.imshow("Line Detection", frame)

        if cv2.waitKey(1) & 0xFF == ord('q'):  # Press 'q' to exit
            break

cap.release()
cv2.destroyAllWindows()

