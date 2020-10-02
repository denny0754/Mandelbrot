# Mandelbrot

This is an implementation of the Mandelbrot Set fractal using C++ and SFML.
This implementation provides the user userful features which are listed below:

 - Single and Multi Threaded Mode:

    - Single-Threaded: Process the Mandelbrot Set point by point on a single thread.
    - Multi-Threaded: Process the Mandelbrot Set in multiple threads. Each thread is assigned with a set of points to process. Note that the number of threads specified by the User to use is not the same amount the Application will use. The total number of threads is actually `MAX_THREADS * MAX_THREADS`. The work on the X axis is calculated as `WINDOW_WIDTH / MAX_THREADS`. The work on the Y axis is calculated as `WINDOW_HEIGHT / MAX_THREADS`. This means each threads has a small rectangle of pixels to process.

 - Sprite and Vertex Buffer Mode:

    - Sprite Mode: Sprite mode is really simple. Each time the Mandelbrot Set is processed, an Image is created and each pixel set. A texture is then created using this Image. The Sprite is loaded with this Texture and finally rendered. This process is a bit longer and slower than using a Vertex Buffer.
    - Vertex Buffer Mode: Unlike a Sprite, Vertex data is stored in graphics memory. If the system support Vertex Buffer and has bottleneck when transferring data from memory to graphics memory, the use of a VertexBuffer could result in a performance gain, expecially in Multi-Threaded mode. Note that this mode can be used ___ONLY___ if the System does support it. In case it's not supported, Sprite Mode will be used.


## Ideas

All ideas listed below could be implemented in a matter of hours, some instead could take days. However, this is just a fun project and a way to me to learn more things and re-learn some other things. So, do not take those ideas as something that I will implement for sure. I might implement them or not. Feel free to fork this repo and implement them yourself if you like. However, if you do, contact me so I can try what I haven't done :)!.

 - Settings: This idea should allow the User to change their settings such as window size, keyboard and mouse bindings and much more. Settings should be stored on a file.
 - Colors: It would be a much more fun experience to allow the User to change what colors to use based on the number of iterations of each pixel. A JSON formatted file should do the job in this case.
 - CUDA: Modern NVidia's GPUs have thousands CUDA Cores, if not more. This is a great techonolgy and would allow to process the Mandelbrot Set in a metter of milliseconds or even less, basically in real time. If used correctly, this idea can out run the CPU's processing power by 10/100/1000 times depending on the GPU. However, because this is a thing of NVidia only GPUs, we cannot use this on AMD GPUs.