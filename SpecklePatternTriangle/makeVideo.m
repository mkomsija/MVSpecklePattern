%small script to make video from individual frames

video = VideoWriter('randomPattern.avi');
video.FrameRate = 5;
open(video);

I = imread('speckles_grande.jpg');
writeVideo(video,I);

I = imread('speckles_grande1.jpg');
writeVideo(video,I);

I = imread('speckles_grande2.jpg');
writeVideo(video,I);

I = imread('speckles_grande3.jpg');
writeVideo(video,I);

I = imread('speckles_grande4.jpg');
writeVideo(video,I);

I = imread('speckles_grande5.jpg');
writeVideo(video,I);

I = imread('speckles_grande6.jpg');
writeVideo(video,I);

I = imread('speckles_grande7.jpg');
writeVideo(video,I);

I = imread('speckles_grande8.jpg');
writeVideo(video,I);

I = imread('speckles_grande9.jpg');
writeVideo(video,I);

I = imread('speckles_grande10.jpg');
writeVideo(video,I);

I = imread('speckles_grande11.jpg');
writeVideo(video,I);

close(video);