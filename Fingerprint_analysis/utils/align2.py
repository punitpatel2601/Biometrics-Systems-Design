# -*- coding: utf-8 -*-
"""
Author: Helder C. R. Oliveira

Date Created: Feb 17, 2021

Email: helder@schulich.ucalgary.ca
"""

import numpy as np

from skimage.util import invert
from skimage.filters import gaussian
from skimage.morphology import dilation, square
from skimage.transform import rotate, rescale

from scipy.signal import correlate2d

import matplotlib.pyplot as plt

import matplotlib as mpl
# setting the default colormap for images only
mpl.rc('image', cmap='gray')


def align2(Fp1, Fp2, plot_res=False):
   
    skel1 = Fp1['thin_image']
    skel2 = Fp2['thin_image']
    
    # Helder: used only later.
    # f = fspecial('gaussian',[8 8],  6); %%% define a special filter

    # dbSkel= imcomplement(full(Fp1.imSkeleton)); % modified by A. Poursaberi
    dbSkel = invert(skel1)
    
    # Helder: dont need to creat a full matrix. minutiate_array is already full
    # dbMinutia = full(Fp1.minutiaArray); %%%Convert sparse matrix to full matrix
    dbMinutia = Fp1['minutiae_array']
    
    #inputSkel= imcomplement(full(Fp2.imSkeleton)); % modified by A. Poursaberi
    inputSkel = invert(skel2)
    
    # Helder: dont need to creat a full matrix. minutiate_array is already full
    #inputMinutia = full(Fp2.minutiaArray);
    inputMinutia = Fp2['minutiae_array']
    
    # se = ones(3); %%%creates 3x3 matrix of 1s
    se = square(3)
    
    # dbMinutia = imdilate(dbMinutia, se); %%%Make the output image the size of 3x3
    dbMinutia = dilation(dbMinutia, se)
    # inputMinutia = imdilate(inputMinutia, se);
    inputMinutia = dilation(inputMinutia, se)
    
    angle = 0
    maxRef = 0
    dwnSampleFactor = 2
    
    # CHECK: I think the data is alreay 'double'
    # inputSkelp = imfilter(double(inputSkel), f); %%%filtering using the filter specified by f
    inputSkelp = gaussian(inputSkel, sigma=6)
    # dbSkelp = imfilter(double(dbSkel), f);    
    dbSkelp = gaussian(dbSkel, sigma=6)
    
    #inputSkelp = inputSkelp/max(inputSkelp(:));
    inputSkelp = inputSkelp / np.max(inputSkelp)
    #dbSkelp = dbSkelp/max(dbSkelp(:));
    dbSkelp = dbSkelp / np.max(dbSkelp)
    
    # rescale allows to set the 'scale', resize doesnt.
    # from the doc: "anti-aliasing should be enabled when down-sizing images to avoid aliasing artifacts"
    
    #inputSkelp = imresize(inputSkelp, 1/dwnSampleFactor);
    inputSkelp = rescale(inputSkelp, scale=1/dwnSampleFactor, anti_aliasing=True)
    #dbSkelp = imresize(dbSkelp, 1/dwnSampleFactor);
    dbSkelp = rescale(dbSkelp, scale=1/dwnSampleFactor, anti_aliasing=True)

    # t1 = size(inputSkelp,1);
    # t2 = size(inputSkelp,2);
    t1, t2 = inputSkelp.shape

    # looking for best translation
    # im1=zeros(size(dbSkel,1),size(dbSkel,2));
    im1 = np.zeros(dbSkel.shape)
    # don't need to take the singularityArray, just the singularity coordinates
    #[x,y]= find(Fp1.singularityArray);
    sing_lst = Fp1['singularities_lst']
    #for i=1:length(x)          
    #    im1(x(i),y(i))=3;
    #end
    for p in sing_lst:
        im1[p[:2]] = 3
    
    #im2=zeros(size(dbSkel,1),size(dbSkel,2));
    im2 = np.zeros(dbSkel.shape)
    #[x,y]= find(Fp2.singularityArray);
    # Feb 22: Fp1 --> Fp2
    sing_lst = Fp2['singularities_lst']
    #for i=1:length(x)          
    #    im2(x(i),y(i))=3;
    #end
    for p in sing_lst:
        im2[p[:2]] = 3
        
    # compute max correlation coefficient
    #c = normxcorr2(im2,im1); 
    c = correlate2d(im2.astype(int), im1.astype(int))
    #[max_cc, imax] = max(abs(c(:)));
    #[ypeak, xpeak] = ind2sub(size(c),imax(1)); 
    # from: https://docs.scipy.org/doc/scipy-0.15.1/reference/generated/scipy.signal.correlate2d.html
    ypeak, xpeak = np.unravel_index(np.argmax(c), c.shape)
    
    #offset = [ (ypeak-size(im1,1)) (xpeak-size(im1,2)) ];
    offset = [ypeak-im1.shape[0], xpeak-im1.shape[1]]
    
    # reconstruction good fp skeletonp translated
    #inputSkelpreconstruct=inputSkelp;
    inputSkelpreconstruct = inputSkelp.copy()
    
    #for i=1:size(inputSkelpreconstruct,1)
    #    for j=1:size(inputSkelpreconstruct,2)
    #        if (i-offset(1)) > 0 & (i-offset(1)) < size(inputSkelpreconstruct,1) & 
    #           (j-offset(2)) > 0 & (j-offset(2)) < size(inputSkelpreconstruct,2)
    #            im(i,j)=inputSkelpreconstruct(i-offset(1),j-offset(2));
    #        else im(i,j)=0;
    #        end
    #    end
    #end
    
    # in MATLAB 'im' is not defined. 
    # In worst case it will have same size as linputSkelpreconstruct
    im = np.zeros(inputSkelpreconstruct.shape)
    for i in range(inputSkelpreconstruct.shape[0]):
        for j in range(inputSkelpreconstruct.shape[1]):
            if ((i-offset[0]) > 0) and ((i-offset[0]) < inputSkelpreconstruct.shape[0]) and \
               ((j-offset[1]) > 0) and ((j-offset[1]) < inputSkelpreconstruct.shape[1]):
                im[i,j] = inputSkelpreconstruct[i-offset[0], j-offset[1]]
            else:
                im[i,j] = 0
    # inputSkelpreconstruct=im;
    inputSkelpreconstruct = im.copy()
    
    # reconstruction good fp skeleton
    #inputSkelreconstruct=inputSkel;
    inputSkelreconstruct = inputSkel.copy()
    # translate (move or shift image) according to offset
    #for i=1:size(inputSkelreconstruct,1)
    #    for j=1:size(inputSkelreconstruct,2)
    #        if (i-offset(1)) > 0 & (i-offset(1)) < size(inputSkelreconstruct,1) & 
    #           (j-offset(2)) > 0 & (j-offset(2)) < size(inputSkelreconstruct,2)
    #            im(i,j)=inputSkelreconstruct(i-offset(1),j-offset(2));
    #        else im(i,j)=0;
    #        end
    #    end
    #end
    
    im = np.zeros(inputSkelreconstruct.shape)
    for i in range(inputSkelreconstruct.shape[0]):
        for j in range(inputSkelreconstruct.shape[1]):
            if ((i-offset[0]) > 0) and ((i-offset[0]) < inputSkelreconstruct.shape[0]) and \
               ((j-offset[1]) > 0) and ((j-offset[1]) < inputSkelreconstruct.shape[1]):
                im[i,j] = inputSkelreconstruct[i-offset[0], j-offset[1]]
            else:
                im[i,j] = 0    
    #inputSkelreconstruct=im;
    inputSkelreconstruct = im.copy()

    # looking for best rotation
    #for ang = -12:3:12
    #    inputSkelr = imrotate(inputSkelpreconstruct, ang, 'crop');
    #    cc = normxcorr2(inputSkelr, dbSkelp);
        # compute max correlation coefficient
        #[max_cc, imax] = max(abs(cc(:)));
        #[rpeak, cpeak] = ind2sub(size(cc),imax(1));
        #corr_offset = [rpeak cpeak];
        #corr_offset = [(rpeak-size(inputSkelr,1)) (cpeak-size(inputSkelr,2))];

        #if max_cc > maxRef
        #    maxRef = max_cc;
        #    offsetrot = corr_offset;
        #    angle = ang;
        #    ccRef = cc;
        #end
    #end
    
    # +1 because otherwise Python wont to return the last angle '12'
    for ang in np.arange(-12, 12+1, 3):
        inputSkelr = rotate(inputSkelpreconstruct, ang)
        cc = correlate2d(inputSkelr, dbSkelp)
        # compute max correlation coefficient
        max_cc = np.max(np.abs(cc))
        rpeak, cpeak = np.unravel_index(np.argmax(cc), cc.shape)
        
        if max_cc > maxRef:
            maxRef = max_cc
            angle = ang
    
    # reconstruction of the  good fp skeleton
    # rotation
    # inputSkelreconstruct=imrotate(inputSkelreconstruct,angle,'crop');
    # Matlab's imrotate returns the image in the same data type as the input.
    # SkImage's rotate, whatever is the input data, the output will be float,
    # unless preserve_range=True
    inputSkelreconstruct = rotate(inputSkelreconstruct, angle, preserve_range=True)
    
    # ------------------------------------------------------------------------------------------------------------
    # for visualization
    # xs = showsimilarity(dbSkel,inputSkelreconstruct);
    # TODO:
    #show_similarity(dbSkel, inputSkelreconstruct)
    
    #inputSkelreconstruct = imcomplement(full(inputSkelreconstruct)); % modified by A. Poursaberi
    inputSkelreconstruct = invert(inputSkelreconstruct)
    
    imSkeletonAlign = inputSkelreconstruct.copy()

    # reconstruct the good minutiae
    # rotation
    #inputMinRec=inputMinutia;
    #inputMinRec=imrotate(inputMinRec,angle,'crop');
    inputMinRec = rotate(inputMinutia, angle, preserve_range=True)
    # translate
    #for i=1:size(inputMinRec,1)
    #    for j=1:size(inputMinRec,2)
    #        if (i-offset(1)) > 0 & (i-offset(1)) < size(inputMinRec,1) & 
    #           (j-offset(2)) > 0 & (j-offset(2)) < size(inputMinRec,2)
    #            im(i,j)=inputMinRec(i-offset(1),j-offset(2));
    #        else im(i,j)=0;
    #        end
    #    end
    #end
    #inputMinRec=im;
    im = np.zeros(inputMinRec.shape)
    for i in range(inputMinRec.shape[0]):
        for j in range(inputMinRec.shape[1]):
            if ((i-offset[0]) > 0) and ((i-offset[0]) < inputMinRec.shape[0]) and \
               ((j-offset[1]) > 0) and ((j-offset[1]) < inputMinRec.shape[1]):
                im[i,j] = inputMinRec[i-offset[0], j-offset[1]]
            else:
                im[i,j] = 0
    inputMinRec = im.copy()
    
    # make superposed skeletons + minutiae
    #[x1,y1]= find(dbMinutia);
    #[x2,y2]= find(im);
    nz_min_coord = np.transpose(np.nonzero(dbMinutia))
    nz_im_coord = np.transpose(np.nonzero(im))

    # TODO: still in test
    if (plot_res):
        #hold on, title('Minutiae')
        #plot(y1,x1,'or','markersize',8)
        #plot(y2,x2,'sb','markersize',8), axis ij
        #legend('Fp1 minutiae', 'Fp2 minutiae')
        #hold off
        for p in nz_min_coord:
            plt.plot(p[0], p[1], 'or', markersize=8)
        for p in nz_im_coord:
            plt.plot(p[0], p[1], 'sb', markersize=8)
    
    #Fp2.minutiaArrayAlign=im;
    minutiaArrayAlign = im.copy()
    
    return imSkeletonAlign, minutiaArrayAlign