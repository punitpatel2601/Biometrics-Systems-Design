# -*- coding: utf-8 -*-
"""
Created on Thu Feb 18 07:38:05 2021

@author: Helder C. R. Oliveira
"""

import numpy as np

from skimage.morphology import dilation, square, binary_erosion
from scipy.ndimage import distance_transform_edt

import matplotlib.pyplot as plt
import matplotlib as mpl
# setting the default colormap for images only
mpl.rc('image', cmap='gray')

def bwdist(img):
    dist, L = distance_transform_edt(np.logical_not(img), return_indices=True)
    return dist, L

# matchingScore = score(dbMinutia ,inputMinutia,dbSkel,inputSkel,threshold2);
def score(db, input, dbSkel, inputSkel, threshold2, plot_res=False):
# function [matchingscore, nbElementInput, nbElement] = score(db, input,dbSkel,inputSkel,threshold2)
    '''
    this function computes the score for the original image in database and
    submitted one, as well as their skeletons 
    threshold2 is added as argument
    '''    
    
    # you may change the threshold. For exmaple, threshold=12, for you Lab exercise
    threshold = threshold2

    #nbElementDb = length(find(db ~= 0));
    #nbElementInput = length(find(input ~= 0));
    nbElementDb = np.count_nonzero(db)
    nbElementInput = np.count_nonzero(input)
    
    # computes the Euclidean distance of the binary image db
    #[Dbdist, L] = bwdist(db); 
    # Matlab to Python: the distance calculation needs the inverted matrix to
    # return same result as Matlab
    Dbdist, L = bwdist(db)
    # and returns the matrices Dbdist and L;
    # in L, each element is the linear index of the nearest nonzero pixel of db.
    #[R, C] = ind2sub([size(L)], L); %%%returns the arrays R and C containing the
    # equivalent row and column subscripts corresponding to the  matrix L
    # for a matrix of the same size.      
    
    #[r, c] = find(input ~= 0); %returns the row and column indices of the nonzero entries in input
    #r = round(r);
    #c = round(c);
    input_coord = np.transpose(np.nonzero(input))
    
    #inputmatch=zeros(size(input,1),size(input,2));
    inputmatch = np.zeros(input.shape)
    #dbmatch=zeros(size(db,1),size(db,2));
    dbmatch = np.zeros(db.shape)
    
    nbmatch = 0
    distot = 0
    
    #for i = 1:length(r)
    #    temp = Dbdist(r(i), c(i));    
    #    if  temp < threshold
    #        input(r(i),c(i))=0;
    #        inputmatch(r(i),c(i)) = 1;
    #        ind = L(r(i), c(i));
    #        db(R(ind), C(ind)) = 0;
    #        dbmatch(R(ind), C(ind)) = 1;
    #        nbmatch = nbmatch + 1;
    #        distot = distot + temp;
    #        [Dbdist, L] = bwdist(db);
    #        %%%fprintf('%d %d - %d %d\n', r(i), c(i), R(ind), C(ind));
    #    end
    #end
    for i in range(input_coord.shape[0]):
        ri, ci = input_coord[i,0], input_coord[i,1]
        temp = Dbdist[ri, ci]
        if temp < threshold:
            input[ri,ci] = 0
            inputmatch[ri,ci] = 1
            R_ind, C_ind = L[0,ri,ci], L[1,ri,ci]
            db[R_ind, C_ind] = 0
            dbmatch[R_ind, C_ind] = 1
            nbmatch += 1
            distot += temp
            Dbdist, L = bwdist(db)
            #Dbdist, L = distance_transform_edt(db, return_indices=True)
    
    #matchingscore = (2 * nbmatch)/(nbElementInput + nbElementDb);
    matchingscore = (2 * nbmatch)/(nbElementInput + nbElementDb)
    
    #xs = showsimilarity(dbSkel,inputSkel);
    #[x1,y1]= find(db);
    coords = np.transpose(np.nonzero(input))
    x1 = [v[0] for v in coords]
    y1 = [v[1] for v in coords]
    #[x2,y2]= find(dbmatch==1);
    x2,y2 = np.where(dbmatch == 1)
    #[x3,y3]= find(input==1);
    x3,y3 = np.where(input == 1)
    #[x4,y4]= find(inputmatch==1);
    x4,y4 = np.where(inputmatch == 1)
    
    # TODO: still in test
    if (plot_res):
        #hold on, title('Minutiae')
        plt.figure()
        #plot(y1,x1,'sb','markersize',8),
        plt.plot(y1, x1, 'sb', markersize=8)
        #plot(y2,x2,'*b','markersize',8),
        plt.plot(y2, x2, '*b', markersize=8)
        #plot(y3,x3,'or','markersize',8),
        plt.plot(y3, x3, 'or', markersize=8)
        #plot(y4,x4,'r*','markersize',8), axis ij
        plt.plot(y4, x4, 'r*', markersize=1)
        plt.gca().invert_yaxis()
        #legend('Fp1 minutiae', 'Fp1 matched minutiae','Fp2 minutiae','Fp2 matched minutiae')
        plt.legend(['Fp1 minutiae', 'Fp1 matched minutiae', 'Fp2 minutiae', 'Fp2 matched minutiae'])
        #xlabel(['Matching Score: ',num2str(matchingscore)])
        plt.xlabel('Matching Score: %.4f' % (matchingscore))
        #hold off
    
    #nbElement = (nbElementInput + nbElementDb);
    nbElement = nbElementInput + nbElementDb
    
    #fprintf('number of matched minutiae : %d\ndistance total computed : %d\n',nbmatch, distot);
    print('number of matched minutiae: %d\ndistance total computed : %d' % (nbmatch, distot))
    #fprintf('number of minutiae in input image : %d\n', nbElementInput);
    print('number of minutiae in input image: %d' % (nbElementInput))
    #fprintf('number of minutiae in database image : %d\n', nbElementDb);
    print('number of minutiae in database image: %d' % (nbElementDb))
    
    return matchingscore, nbElementInput, nbElement



# Score2=match(Fp1.minutiaArray, Fp2.minutiaArrayAlign, Fp1.imSkeleton, Fp2.imSkeletonAlign,threshold2);
def match(Fp1, Fp2, threshold2=1):
# matchingScore = match(dbMinutia,inputMinutia,dbSkel,inputSkel,threshold2)
    '''
    the function's arguments are the database vector of minutiae and the
    submitted (probed) vector of minutiae to match them, as well as the database vector
    representing skeleton and the vector representing the skeleton of the submitted one.
    threshold2 is added as a parameter on Feb.04, 2009
    '''
    
    dbMinutia = Fp1['minutiae_array'].copy()
    inputMinutia = Fp2['minutiaArrayAlign'].copy()
    
    dbSkel = Fp1['thin_image'].copy()
    inputSkel = Fp2['imSkeletonAlign'].copy()
    
    
    
    #### FIX: THe following dilation operations creates more MINUTIAE!!!!!!!
    
    
    
    #se = ones(3);
    #dbMinutia = imdilate(dbMinutia, se); 
    # dilates the grayscale image
    se = square(3)
    dbMinutia = dilation(dbMinutia, se)
    # se is a 3x3 array of ones, so it dilates the image to 9 squares

    # inputMinutia = imdilate(inputMinutia, se);    
    inputMinutia = dilation(inputMinutia, se)
    
    #  morphological operation of shrinking bdMinutia to points
    #dbMinutia = bwmorph(dbMinutia, 'Shrink', 'Inf');
    # TODO: check if the image has the object in black or white
    # in Matlab the object is white. However the use of this function does nothing
    # in Matlab because the dbMinutia has only white dots
    dbMinutia = binary_erosion(dbMinutia)
    # now the image dbMintia is logical
    
    #inputMinutia = bwmorph(inputMinutia, 'Shrink', 'Inf');
    inputMinutia = binary_erosion(inputMinutia)
    # now the image inputMintia is logical
    
    # matchingScore = score(dbMinutia ,inputMinutia,dbSkel,inputSkel,threshold2);
    matchingScore,_,_ = score(dbMinutia, inputMinutia, dbSkel, inputSkel, threshold2)
    
    return matchingScore
