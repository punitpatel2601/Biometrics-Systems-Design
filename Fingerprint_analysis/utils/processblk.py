# -*- coding: utf-8 -*-
"""
Author: Helder C. R. Oliveira

Date Created: Feb 05, 2020

Email: helder@schulich.ucalgary.ca
"""

import numpy as np

def processblk(img, blkSize=16):
    r, c = img.shape

    img_bin = np.zeros(img.shape)

    # process the image based on blkSize
    for i in range(0, r, blkSize):
        for j in range(0, c, blkSize):
            img_roi = img[i:i+blkSize, j:j+blkSize]
            [fy, fx] = np.gradient(img_roi, blkSize)

            gradDev = (np.std(fx) + np.std(fy)) * 1000;

            if (gradDev > 4):   # 30 good for IPS images
                img_bin[i:i+blkSize, j:j+blkSize] = np.ones(img_roi.shape)
            else:
                img_bin[i:i+blkSize, j:j+blkSize] = np.zeros(img_roi.shape)

    return img_bin