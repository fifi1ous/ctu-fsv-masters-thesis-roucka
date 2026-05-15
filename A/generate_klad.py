import os
import random
from PIL import Image, ImageDraw, ImageFont

# --- 0. Configuration ---
# path to the output directory
OUTPUT_DIR = "generated_klad_dilu"
# number of images to generate
NUM_IMAGES = 1300

# A4 dimensions in pixels (96 DPI)
A4_PORTRAIT_W  = 1024
A4_PORTRAIT_H  = 1448
A4_LANDSCAPE_W = 1448
A4_LANDSCAPE_H = 1024

# probability of landscape (25%) vs portrait (75%)
LANDSCAPE_PROBABILITY = 0.25

# minimum number of sheets
MIN_SHEETS = 3
# maximum number of sheets
MAX_SHEETS = 10


BG_COLOR   = (255, 255, 255)
LINE_COLOR = (0, 0, 0)
TEXT_COLOR = (0, 0, 0)

# title text
TITLE_TEXT = "KLAD MAPOVÝCH DÍLŮ"

# fixed size of the rectangle (px)
RECT_W = 300
RECT_H = 440

# minimum gap between rectangles (px)
MIN_GAP = -100

# maximum gap from the edges (px)
MAX_GAP = 40

# maximum allowed gap between adjacent rectangles (px)
MAX_GAP_BETWEEN_RECTS = 20


# line width (px)
LINE_WIDTH = 1
# top margin for the title (px)
TOP_MARGIN_FOR_TITLE = 50

# maximum number of attempts to place a rectangle without collision
MAX_PLACEMENT_ATTEMPTS = 200

# YOLO class
CLASS_KLAD = 5

# --- Helper functions ---
# // this part of code was modified by Claude 4.7 opus model //
# helper function to check if a rectangle has a collision with another rectangle
def has_collision(new_rect, placed_rects, gap):
    # get the coordinates of the new rectangle
    nx1, ny1, nx2, ny2 = new_rect
    # get the coordinates of the new rectangle with the gap
    nx1g, ny1g, nx2g, ny2g = nx1 - gap, ny1 - gap, nx2 + gap, ny2 + gap
    # check if the new rectangle has a collision with any of the placed rectangles
    for px1, py1, px2, py2 in placed_rects:
        # check if the new rectangle has a collision with the placed rectangle
        if not (nx2g < px1 or px2 < nx1g or ny2g < py1 or py2 < ny1g):
            # return True if the new rectangle has a collision with the placed rectangle
            return True
    # return False if the new rectangle does not have a collision with any of the placed rectangles
    return False

# helper function to calculate the minimum distance to a placed rectangle
def min_dist_to_placed(new_rect, placed_rects):
    # get the coordinates of the new rectangle
    nx1, ny1, nx2, ny2 = new_rect
    # initialize the minimum distance to infinity
    min_dist = float("inf")
    # calculate the minimum distance to a placed rectangle
    for px1, py1, px2, py2 in placed_rects:
        # calculate the distance between the new rectangle and the placed rectangle
        dx = max(px1 - nx2, nx1 - px2, 0)
        # calculate the distance between the new rectangle and the placed rectangle
        dy = max(py1 - ny2, ny1 - py2, 0)
        # calculate the distance
        dist = (dx ** 2 + dy ** 2) ** 0.5
        # update the minimum distance
        min_dist = min(min_dist, dist)
    # return the minimum distance
    return min_dist
# // end of code modified by Claude 4.7 opus model //

# helper function to convert a bounding box to YOLO format  
def to_yolo(bbox, img_width, img_height):
    # get the coordinates of the bounding box
    x1, y1, x2, y2 = bbox
    # calculate the center of the bounding box
    cx = (x1 + x2) / 2 / img_width
    # calculate the center of the bounding box
    cy = (y1 + y2) / 2 / img_height
    # calculate the width of the bounding box
    w  = (x2 - x1) / img_width + 1e-2
    h  = (y2 - y1) / img_height + 1e-2
    # return the center of the bounding box, width, and height
    return cx, cy, w, h


# --- 1. Main part of the script ---
# create the output directory if it does not exist
os.makedirs(OUTPUT_DIR, exist_ok=True)

# load the default font
TITLE_FONT = ImageFont.load_default()
# load the default font
LABEL_FONT = ImageFont.load_default()
# loop through the number of images

# loop through the number of images
for index in range(NUM_IMAGES):
    # select the orientation of the image
    if random.random() < LANDSCAPE_PROBABILITY:
        img_width, img_height = A4_LANDSCAPE_W, A4_LANDSCAPE_H
    else:
        img_width, img_height = A4_PORTRAIT_W, A4_PORTRAIT_H

    # create a new image with the background color
    image = Image.new("RGB", (img_width, img_height), BG_COLOR)
    # create a new image draw object
    draw  = ImageDraw.Draw(image)

    # create a list to store all bounding boxes
    all_bboxes = []

    # calculate the bounding box of the title
    title_bbox = draw.textbbox((0, 0), TITLE_TEXT, font=TITLE_FONT)
    # calculate the width and height of the title
    tw = title_bbox[2] - title_bbox[0]
    th = title_bbox[3] - title_bbox[1]
    # calculate the x and y position of the title
    title_x = (img_width - tw) // 2
    # calculate the y position of the title
    title_y = 70
    # draw the title on the image
    draw.text((title_x, title_y), TITLE_TEXT, fill=TEXT_COLOR, font=TITLE_FONT)
    # add the bounding box of the title to the list of all bounding boxes
    all_bboxes.append([title_x, title_y, title_x + tw, title_y + th])

    # calculate the weights for the number of sheets
    weights = [2 ** (MAX_SHEETS - n) for n in range(MIN_SHEETS, MAX_SHEETS + 1)]
    # select the number of sheets
    num_sheets = random.choices(range(MIN_SHEETS, MAX_SHEETS + 1), weights=weights, k=1)[0]

    # generate the rectangles without collision
    rects = []

    # loop through the number of sheets
    for _ in range(num_sheets):
        # initialize the rectangle to None
        rect = None
        # loop through the maximum number of attempts to place a rectangle without collision
        # // this part of code was modified by Claude 4.7 opus model //
        for _ in range(MAX_PLACEMENT_ATTEMPTS):
            # generate the x and y position of the rectangle
            x1 = random.randint(MAX_GAP, img_width  - RECT_W - MAX_GAP)
            # generate the y position of the rectangle
            y1 = random.randint(TOP_MARGIN_FOR_TITLE + MAX_GAP, img_height - RECT_H - MAX_GAP)
            # create a candidate rectangle
            candidate = [x1, y1, x1 + RECT_W, y1 + RECT_H]
            # check if the candidate rectangle has a collision with any of the placed rectangles
            if has_collision(candidate, rects, MIN_GAP):
                continue
            # check if the candidate rectangle is too far from the placed rectangles
            if rects and min_dist_to_placed(candidate, rects) > MAX_GAP_BETWEEN_RECTS:
                continue
            # set the rectangle to the candidate rectangle
            rect = candidate
            # break the loop if the rectangle is placed
            break
        # // end of code modified by Claude 4.7 opus model //
        
        # check if the rectangle is not None
        if rect is not None:
            # add the rectangle to the list of rectangles
            rects.append(rect)


    # draw all rectangles
    for x1, y1, x2, y2 in rects:
        # draw the rectangle on the image
        draw.rectangle([x1, y1, x2, y2], outline=LINE_COLOR, width=LINE_WIDTH)

        all_bboxes.append([x1, y1, x2, y2])

    # draw the descriptions of the rectangles
    for i, (x1, y1, x2, y2) in enumerate(rects):
        # generate the label of the rectangle
        label = f"Díl {i + 1}"

        # select the corner of the label
        corner = random.choice(["top_left", "top_right"])
        # if the corner is top_left, set the x and y position of the label
        if corner == "top_left":
            # set the x and y position of the label
            tx = x1 + 14
            # set the y position of the label
            ty = y1 + 10
        # if the corner is top_right, set the x and y position of the label
        else:
            # calculate the bounding box of the label
            tb = draw.textbbox((0, 0), label, font=LABEL_FONT)
            # set the x position of the label
            tx = x2 - (tb[2] - tb[0]) - 14
            # set the y position of the label
            ty = y1 + 10

        # draw the label on the image
        draw.text((tx, ty), label, fill=TEXT_COLOR, font=LABEL_FONT)
        # calculate the bounding box of the label
        label_bbox = list(draw.textbbox((tx, ty), label, font=LABEL_FONT))

    # calculate the bounding box of the all rectangles
    if all_bboxes:
        # calculate the minimum x position of the all rectangles
        min_x = min(b[0] for b in all_bboxes)
        # calculate the minimum y position of the all rectangles
        min_y = min(b[1] for b in all_bboxes)
        # calculate the maximum x position of the all rectangles
        max_x = max(b[2] for b in all_bboxes)
        # calculate the maximum y position of the all rectangles
        max_y = max(b[3] for b in all_bboxes)
        # calculate the center of the bounding box of the all rectangles
        cx, cy, w, h = to_yolo([min_x, min_y, max_x, max_y], img_width, img_height)
        # generate the YOLO line
        yolo_line = f"{CLASS_KLAD} {cx:.6f} {cy:.6f} {w:.6f} {h:.6f}"
    else:
        # set the YOLO line to empty string
        yolo_line = ""

    # save the image and the YOLO file
    base_name = f"klad_dilu_{index:05d}"
    # save the image
    image.save(os.path.join(OUTPUT_DIR, f"{base_name}.png"))

    # save the YOLO file
    with open(os.path.join(OUTPUT_DIR, f"{base_name}.txt"), "w", encoding="utf-8") as f:
        f.write(yolo_line)

# print a message that the images have been generated
print(f"Hotovo. Vygenerováno {NUM_IMAGES} obrázků do složky: {OUTPUT_DIR}")
