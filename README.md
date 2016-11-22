openframeworks app (W.I.P.) to do real-time reverse image search.

this doesn't work properly yet.

requires:
 - [ofxCcv](https://github.com/kylemcdonald/ofxCcv)
 - [ofxPCA](https://github.com/genekogan/ofxPCA)
 - [ofxKDTree](https://github.com/genekogan/ofxKDTree)
 - [ofxJSON](https://github.com/jefftimesten/ofxJSON)

Analysis
1) extract feature vectors for a large collection of images using ofxCcv.
2) compress the feature vectors using PCA and save the principal components.
3) store transformed feature vectors in a KDTree and corresponding image paths, and save everything

Run-time
1) Load KDTree and principal components to memory
2) continuously analyze webcam stream or video player with convnet
3) compress feature vectors using principal components from #2
4) retrieve k-nearest neighbors from kd-Tree and load images to memory

Issues:
 - PCA takes too long
 - KDTree implementation not verified working right yet...

Todo:
 - at least recreate quality of [ReverseImageSearch](https://github.com/ml4a/ml4a-ofx/tree/master/apps/ReverseImageSearch)
 - verify good hashing via KDTree