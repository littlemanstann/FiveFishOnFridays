# FiveFishOnFridays
Our fishy Final Group Project submission for COMP 3501! <br>
*BY: Justin, Jansen, Jessica, Stanny*

**REQUIRED ADDONS: ofxAssimpModelLoader**

### Project Setup

1. Download the project .zip file from Brightspace
2. Unzip the .zip file into a new folder in /<of_folder_location>/apps/myApps/
3. Open /<of_folder_location>/projectGenerator/projectGenerator.exe
4. Click the "import" button and navigate to where you unzipped your submission in /myApps/
    4a. Ensure that the 
5. Click "Generate" (or if the button prompts change, click "Update")
6. projectGenerator.exe should now prompt you to open the project. Click the button "Open in IDE"
7. In Visual Studio 2022, go to the right hand side 'Solution Explorer' and click the 'Show All Files' icon; it should look like overlapping rectangles
8. Make sure every .cpp and .h file is included in the solution (right click file and choose 'Include In Project' if not)
9. Run the project (click the green "play" button)

### Controls
**L** - Lock mouse to to screen for camera control<br>
**W** - Move fish forwards <br>
**S** - Move fish backwards <br>
**LEFT_SHIFT** or **R** - Toggle dash (move fast) <br>
**E** - Interact and engage in dialogue with other fish (must be near + facing fish) <br>
**SPACEBAR** - Continue through dialogue boxes

### Game Architecture
All player movement and camera controls are handled within *MyCustomCamera.cpp*. <br> Hierarchical fish transformations are handled in *Player.cpp*. <br> 
*MeshObject.h* and the associated files in *"/MeshObject Primitives"* are helper files for our hierarchal transformations. <br> 
*BubbleEmitter.cpp* and *SeaweedBall.cpp* contain our particle system code. <br> *WaterDroplet.cpp* handles our different water-droplet worlds you can traverse through. <br> 
The 3D Voronoi Mesh is generated using *3DPoint.cpp*. <br>
The dialogue GUI and sounds are handled in *DialogueBox.cpp*.