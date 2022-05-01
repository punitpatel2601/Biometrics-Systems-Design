# -*- coding: utf-8 -*-
"""
Created on Mon Feb 10 15:42:03 2020

@author: Helder

Email: helder@schulich.ucalgary.ca
"""

import numpy as np
import matplotlib.pyplot as plt
from scipy import ndimage as ndi
from scipy.spatial.distance import euclidean
from skimage.filters import gabor_kernel
from skimage.transform import resize

from utils.blockproc import blockproc

def MatchGaborFeat(im1, im2, plot_res=False):
    
    # unniform image size
    #im2 = resize(im2, im1.shape, preserve_range=True)
    
    r1,c1 = im1.shape
    r2,c2 = im2.shape
    
    # sizing a part of the image; you can choose other size here
    ss = 48 
    # crop out part of the core area
    im1 = im1[ss:r1-ss, ss:c1-ss]
    # crop out part of the core area
    im2 = im2[ss:r2-ss, ss:c2-ss]
    
    r,c = im1.shape
     
    if plot_res:        
        # display both compared fingerprint fragments
        plt.figure(figsize=(8,12))
        
        plt.subplot(1,2,1)
        plt.imshow(im1, cmap='gray')
        plt.title('Fp1')
        
        plt.subplot(1,2,2)
        plt.imshow(im2, cmap='gray')
        plt.title('Fp2')

    # define the parameters for the gabor filters
    norient = 8
    # filter size: x
    xsize = 32
    # filter size: y
    ysize = 32
    # standard deviation of the gaussian envelope: x
    dx = 8
    # standard deviation of the gaussian envelope: y
    dy = 4
    # frequency
    fq = 0.11
    
    # +0.1 because python discart the last value
    angle = np.arange(0, np.pi-np.pi/8+0.1, np.pi/8)
    
    # alocating space for the results
    If1 = []
    If2 = []
    F1 = []
    F2 = []
    f = []
    
# %define the features - absolute deviation from the mean
# %%% aad = @(x) abs(mean2(x)-std2(x)); 
# aad=inline('abs(mean2(x)-std2(x))');  %%%inline(expression) 
# %%%constructs an inline function object from the expression.  x is a default argument 

    if plot_res:
        plt.figure(figsize=(14,8))
                
    # filtering and feature extraction
    for i,a in enumerate(angle):
        # build the filter
        gabor = np.real(gabor_kernel(fq, theta=a, sigma_x=dx, sigma_y=dy, n_stds=4))
        #gabor = resize(gabor, (xsize, ysize), mode='constant', preserve_range=True)
        f.append(gabor)

        # filter I1 and I2
        If1.append(ndi.convolve(im1, f[i], mode='wrap'))
        If2.append(ndi.convolve(im2, f[i], mode='wrap'))
        
        #print('If1: ', np.min(im1[i]), np.max(im1[i]))
        #print('If2: ', np.min(im2[i]), np.max(im2[i]))
        
        # extract features
        F1.append(blockproc(If1[i], 16, 16))
        F2.append(blockproc(If2[i], 16, 16))
        
        if plot_res:
            # normalization to plot
            F1_plot = ((F1[i] - np.min(F1[i])) / (np.max(F1[i]) - np.min(F1[i]))) * 255
            F2_plot = ((F2[i] - np.min(F2[i])) / (np.max(F2[i]) - np.min(F2[i]))) * 255
            
            # show Gabor filters+features
            plt.subplot(3, norient, i+1)
            plt.imshow(f[i], cmap='gray', aspect='auto')
            plt.tick_params(left=False, bottom=False, labelleft=False, labelbottom=False)
            plt.subplot(3, norient, i+1+norient)
            plt.imshow(F1_plot, cmap='gray', aspect='auto')
            plt.tick_params(left=False, bottom=False, labelleft=False, labelbottom=False)
            plt.subplot(3, norient, i+1+2*norient)
            plt.imshow(F2_plot, cmap='gray', aspect='auto')
            plt.tick_params(left=False, bottom=False, labelleft=False, labelbottom=False)
            
        
    # reshape features into a vector (eg. to classify using any 
    # classifier that operates on fixed-length feature vectors)
    F1_v = []
    F2_v = []
    
    #print(len(F1), F1[0].shape)
    #print(len(F2), F2[0].shape)
    
    for i,_ in enumerate(F1):
        F_sz = F1[i].shape
        F_sz = np.prod(F_sz)
        
        F1_v = np.concatenate((F1_v, np.reshape(F1[i], (F_sz,1)).squeeze()))
        F2_v = np.concatenate((F2_v, np.reshape(F2[i], (F_sz,1)).squeeze()))
        
    
    #print(F1_v.shape)
    #print(F2_v.shape)
                
    # compute score as the mean of the Euclidian distance between 2 feature vectors
    D = np.abs(F1_v - F2_v)
    score = np.mean(D)
    
    return score