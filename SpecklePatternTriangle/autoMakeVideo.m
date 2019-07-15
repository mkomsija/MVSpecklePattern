%small script to make video from individual images

video = VideoWriter('test4/randomPattern.avi');
video.FrameRate = 2;
open(video);

for i=0:10
    name = ['test4/',num2str(i), '.jpg'];
    writeVideo(video,imread(name));
end
    

close(video);