# -*- coding: utf-8 -*-
"""
Created on Wed Feb 17 15:22:49 2021

@author: Helder C. R. Oliveira
"""

import cv2 as cv
import numpy as np
from scipy.ndimage import distance_transform_edt


def minutiae_at(pixels, i, j, kernel_size):
    """
    https://airccj.org/CSCP/vol7/csit76809.pdf pg93
    Crossing number methods is a really simple way to detect ridge endings and ridge bifurcations.
    Then the crossing number algorithm will look at 3x3 pixel blocks:

    if middle pixel is black (represents ridge):
    if pixel on boundary are crossed with the ridge once, then it is a possible ridge ending
    if pixel on boundary are crossed with the ridge three times, then it is a ridge bifurcation

    :param pixels:
    :param i:
    :param j:
    :return:
    """
    # if middle pixel is black (represents ridge)
    if pixels[i][j] == 1:

        if kernel_size == 3:
            cells = [(-1, -1), (-1, 0), (-1, 1),        # p1 p2 p3
                   (0, 1),  (1, 1),  (1, 0),            # p8    p4
                  (1, -1), (0, -1), (-1, -1)]           # p7 p6 p5
        else:
            cells = [(-2, -2), (-2, -1), (-2, 0), (-2, 1), (-2, 2),                 # p1 p2   p3
                   (-1, 2), (0, 2),  (1, 2),  (2, 2), (2, 1), (2, 0),               # p8      p4
                  (2, -1), (2, -2), (1, -2), (0, -2), (-1, -2), (-2, -2)]           # p7 p6   p5

        values = [pixels[i + l][j + k] for k, l in cells]

        # count crossing how many times it goes from 0 to 1
        crossings = 0
        for k in range(0, len(values)-1):
            crossings += abs(values[k] - values[k + 1])
        crossings //= 2

        # if pixel on boundary are crossed with the ridge once, then it is a possible ridge ending
        # if pixel on boundary are crossed with the ridge three times, then it is a ridge bifurcation
        if crossings == 1:
            return "ending"
        if crossings == 3:
            return "bifurcation"

    return "none"


def calculate_minutiae(im, mask, kernel_size=3):
    # Helder: modification to return the list of minutiae coordinates
    
    biniry_image = np.zeros_like(im)
    biniry_image[im<10] = 1.0
    biniry_image = biniry_image.astype(np.int8)
    
    minutiae_array = np.zeros_like(im)

    (y, x) = im.shape
    result = cv.cvtColor(im, cv.COLOR_GRAY2RGB)
    colors = {"ending" : (150, 0, 0), "bifurcation" : (0, 150, 0)}

    mask_dist = distance_transform_edt(mask)
    
    minutiae_lst = []

    # iterate each pixel minutia
    for i in range(1, x - kernel_size//2):
        for j in range(1, y - kernel_size//2):          
            dist = mask_dist[j,i] 
            minutiae = minutiae_at(biniry_image, j, i, kernel_size)
            # Helder: 'dist' > 30 to discart minutiae close to the border
            if minutiae != "none" and dist > 30:
                cv.circle(result, (i,j), radius=2, color=colors[minutiae], thickness=2)
                minutiae_lst.append([i,j,minutiae])

    for p in minutiae_lst:
        r,c = p[0],p[1]
        min_type = p[2]
        
        # Helder: set the minutiae array to be used by the matching process
        if (min_type == 'ending'):
            minutiae_array[c,r] = 1
        elif (min_type == 'bifurcation'):
            minutiae_array[c,r] = 2
        else:
            minutiae_array[c,r] = 0

    return minutiae_lst, result, minutiae_array
