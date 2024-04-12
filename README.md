Template code for A04
------------

Author: Joshua Baus [joshuabaus@arizona.edu]
<br>
Date: April 12, 2024

<br>
Bugs: <br>
As far as I know, there are none. At very low wavelengths, the image looks a little goofy but thats probably due to how fast the oscilations are. <br>
Same with very high amplitudes, the image looks weird because of how high the derivative is.

<br>
<br>
Features: <br>
Press the 'd' key to start the waves <br>
Use the 'space' and 'left shift' keys to adjust the rr value. <br>
Use the 'left arrow' and 'right arrow' keys to adjust the wavelength<br>
Use the 'up arrow' and 'down arrow' keys to adjust the amplitude<br>
The wavelength and amplitude can be negative and zero. This was kept for testing purposes but in the real world
Those should not be possible<br>
<br>
Behaviors:<br>
As noted in the code, I am using a skybox, but I only use the 'up' texture for all faces of the cube. It looks better to me to have the sky for all reflections because the ground surface looked weird on the water. I understand that this is not how skyboxes work exactly but it looks better this way.
<br> Also, while adjusting the wavelength, the waves move really fast and look weird. This is due to the rate at which I can adjust the waves. So if you see that in the videos, its due to me adjusting the waves and not a bug.


<br>
<br>
To run the program: <br>
First use make to build the executable and then run it using ./test as 'test' is the name of the executable. <br>
A window should appear with an 'F' in the center. But due to github not allowing me to upload files over 25M, I was unable to upload all of my dependencies. So downloading the repo will not work as expected. I can come into office hours to show you the code and project working if necessary. I also will attach several videos and images of it working for your ease.

<br>
<br>
Demos/Examples:
In the 'Demos' folder are videos and pictures of my program working.
<br>

<br>
<br>
Lighting effects: <br>
As specified, I implemented diffuse lighting along with reflections. I believe it looks correct. <br>

<br>
<br>
Attribution:<br>
f-texture.png is from webglfundamentals.org<br>
skybox: Sky/Cloud boxes created by Zachery "skiingpenguins" Slocum (freezurbern@gmail.com) http://www.freezurbern.com
<br>
