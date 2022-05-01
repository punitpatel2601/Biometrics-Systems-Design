# -*- coding: utf-8 -*-
"""
Created on Mon Feb 10 16:23:46 2020

@author: Helder

Email: helder@schulich.ucalgary.ca
"""

import numpy as np
from skimage.util.shape import view_as_blocks
# pad removed from Sk-Image 0.19 onwards 
#from skimage.util import pad

def blockproc(img, xsize=16, ysize=16):
    img_bak = img.copy()
    r, c = img.shape
    
    #xstep = int(np.floor(xsize/2))
    #ystep = int(np.floor(ysize/2))
    
    #img_f = np.zeros((ystep,img.shape[1]))
    
    xstep = int(np.ceil(c/xsize))
    ystep = int(np.ceil(r/ysize))
    
    pad_x, pad_y = xstep*16-c, ystep*16-r

    # 'row' vs. 'columns' is different when thinking about 'x' vs. 'y'
    res = np.zeros((ystep,xstep))
    
    im1 = np.pad(img_bak, 
                 pad_width=((0,pad_y), (0,pad_x)), 
                 mode='reflect')
    
    #OLD: img = np.pad(img, ((0, ystep), (0, xstep)), 'symmetric')
    im_patch = view_as_blocks(im1, block_shape=(ysize, xsize))
    d1,d2,_,_ = im_patch.shape
    
    # process the image based on blkSize
    for i in range(d1):
        for j in range(d2):
            roi = im_patch[i,j,:,:]
            # aad = inline('abs(mean2(x)-std2(x))');
            res[i,j] = np.abs(np.mean(roi) - np.std(roi))


    return res

    
    
    #for j in range(0, c, xstep):
    #    r_idx = 0
    #    for i in range(0, r, ystep):
    #        img_roi = img[i:i+ystep, j:j+xstep]

            #img_f[r_idx:r_idx+ystep, j] = np.abs(np.mean(img_roi) -  np.std(img_roi))
            #r_idx += 1
    #return img_f