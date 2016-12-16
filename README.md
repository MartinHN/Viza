#ViZa

tool for visual exploration of music


ViZa
====

We implemented a visualization tool, “ViZa”, enabling to show 3d relationship between aggregated features of audio samples. This application was developed for use in a research context, specifically for short audio samples (< 10s), with the following goals:
to rapidly visualize distribution and correlation of features
to quickly spot outliers, in a straight way or based on t-sne’s dimension reduction algorithm
to check perceptual validity of feature distribution by playing instantaneously with the mouse in the 3d space
to generate user test validation of extracted features, as a simple satisfaction study on given axes and cluster distribution, or by allowing them to modify the underlying 3d space for iterative research

A self - explanatory video can be seen at : http://youtu.be/iF8_3dIZ_34 


Run Examples
=====
* unzip example folder
* launch Viza.app
* select example/annotation folder when asked
* wait a bit ...


dependencies
=====
essentia : branch Viza
essentiaX : https://github.com/MartinHN/essentiaX
and openFrameworks : https://github.com/MartinHN/openFrameworks.git : on branch Viza

#documentation can be found [here](https://drive.google.com/drive/folders/0B_ZMqbx9jXAud0dENk1ZNE1QakU?usp=sharing)