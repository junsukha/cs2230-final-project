# Projects 5 & 6: Lights, Camera & Action!

All project handouts can be found [here](https://cs1230.graphics/projects).


# Extra Credits.
## EC 1: Distance from the object to the camera: search for "//extra credit 2"
I used settings.extracredit2.
I calculated distance using camera's position and the center of object, both in the world space. As the range of paramters (0-25) and that of distance (0-100) are significantly different, I rescaled distance by mulitplying (5 / 100). So, I divide ths parameters by (distance * 5 / 100).
    
## EC 2: The number of objects in the scene: search for "//extra credit 1"
I used settings.extracredit1.
To make sure the difference is visible, I multiplied parameters by 2 and then divided them by the number of objects in the scene.

## EC 3: object reader
I added a file "objreader.h" to read/prase .obj.  Also search for "// extra credit 3" for related code.
objreader.h has multiple definition errors. CMakefile has the same definitions for some variables. initializeMesh() is related with objreader.h and initializeMesh2() is related with objreader2.h. objreader2.h is another obj parser/reader. 
paintMesh() paints objects. I had to comment the relevant codes because otherwise it throws errors.

For Project 6, I use m_defaultFBO = 2

## EC 4: texture mapping
This is extra credit 4.
-Search for "settings.extraCredit4" or "// texture mapping". I tried to save each shape's texturemap to m_shape_texture. Likewise drawing myFBO to default FBO, I tried to use sampling in default.frag (where I draw scene to myFBO) and sample from shape's texture. My idea is that, as we sample from myFBO texture to draw on defaultFBO to show on the screen, I can sample from shape texture when I calculate light color in myFBO so that I can blend the two.
-Add uv coordinates to each shape vertex points
-Save each shape's texture map to m_shape_texture
-My second logic is that I save my shape texture in myFBO and sample from it using scene instead of full quad. When drawing a scene, I sample from shape texture in myFBO and blend it with light color.


## EC 5: gray scale
Search for "// gray scale extra credit" in postprocessing.frag and uncomment the relevant lines. I'm currently using "invert". Use per-pixel button on gui to activate "invert" or "gray".

## EC 6: kernel filter edge dection
Search for "kernel filter extra credit" in realtime.cpp. Currently using uniformSharpenFilter(). Should comment uniformSharpenFilter() and uncomment uniformEdgeFilter() to use edge detection filter. Use kernel filter button on the window to apply the filter.

## EC 7:FAXX filtering
Search for "FXAA extra credit" in postprocessing.frag
Uncomment the line "fragColor = FXAA();" and run. The function's name is FXAA().

