# Tucil 2
<div align="justify"> This repository contains an image compression application that leverages a Quadtree data structure, built in <code>C++</code> using <code>OpenCV</code> and <code>ImageMagick</code> that implements various <strong>Divide and Conquer</strong> and <strong>Decrease and Conquer</strong> algorithms. This program also incorporates multiple error measurement methods—including variance, mean absolute deviation (MAD), maximum pixel difference, entropy, and Structural Similarity Index (SSIM)—to decide when to subdivide image regions. Additionally, it can generate Graphics Interchange Format (GIF) files to visualize the compression process, providing an intuitive demonstration of how the quadtree structure evolves during compression. </div>
 
 ---
<div align="center">
  <img src="https://github.com/user-attachments/assets/3f986e00-4d65-4c02-8461-cc539b5f3fda" alt="Lock-in" />
</div>

 <!-- CONTRIBUTOR -->
 <div align="center" id="contributor">
   <strong>
     <h3>~ Rudal Sekeloa Reloaded 🚀 ~</h3>
     <table align="center">
       <tr align="center">
         <td>NIM</td>
         <td>Nama</td>
         <td>GitHub</td>
       </tr>
       <tr align="center">
         <td>13523027</td>
         <td>Fajar Kurniawan</td>
         <td><a href="https://github.com/Fajar2k5">@Fajar2k5</a></td>
       </tr>
       <tr align="center">
         <td>13523090</td>
         <td>Nayaka Ghana Subrata</td>
         <td><a href="https://github.com/Nayekah">@Nayekah</a></td>
       </tr>
     </table>
   </strong>
 </div>
 
 <div align="center">
   <h3 align="center">~ Tech Stacks and Languages ~ </h3>
 
   <p align="center">
 
[![C++](https://img.shields.io/badge/C++-00599C.svg?style=for-the-badge&logo=cplusplus&logoColor=white)](https://isocpp.org)
[![OpenCV](https://img.shields.io/badge/OpenCV-5C3EE8.svg?style=for-the-badge&logo=opencv&logoColor=white)](https://opencv.org)
[![ImageMagick](https://img.shields.io/badge/ImageMagick-000000.svg?style=for-the-badge&logo=imagemagick&logoColor=white)](https://imagemagick.org)
 
   </p>
 </div>
 
 ---
 
 ## 📦 Installation & Setup
 
 ### ✅ Requirements
 - Git
 - Any IDE (recommended: VSCode)
 - Chocolatey (for Windows)
 - CMake
 - OpenCV
 - ImageMagick
 - C++ Compiler
 
 ### ⬇️ Installing Dependencies
 
 #### 🪟 Windows
 1. Git
       ```bash
    https://git-scm.com/download/win
    ```
 2. VSCode
       ```bash
    winget install microsoft.visualstudiocode
    ```
 3. Mingw (C++ Compiler)
       ```bash
    # Install and setup for Windows (see the docs below)
       
    https://www.mingw-w64.org/downloads/
    ```
 4. Chocolatey (Package installer for Windows)
       ```bash
    # Install and setup (see the docs below)
       
    https://chocolatey.org/install
    ```
5. Dependecies
    ```bash
    # Run it in administrator mode (cmd/powershell)
    
    choco install cmake -y
    choco install opencv -y
    choco install imagemagick -y
    ```

 <br/>
 <br/>
 
 #### 🐧 Linux
 1. Git
       ```bash
    sudo apt-get install git
    ```
 2. VSCode
       ```bash
    sudo apt install software-properties-common apt-transport-https wget
    wget -q https://packages.microsoft.com/keys/microsoft.asc -O- | sudo apt-key add -
    sudo add-apt-repository "deb [arch=amd64] https://packages.microsoft.com/repos/vscode stable main"
    sudo apt update
    sudo apt install code
    ```
4. Dependecies
    ```bash
    sudo apt update
    sudo apt install cmake
    sudo apt install libopencv-dev
    sudo apt install imagemagick
    ```
 
 ---
 ## 🔧 How To Setup
 
 ### **🪟 Windows**
 #### 🛠️ Build (If you want to build it from the beginning)
 1. Open a terminal
 2. Clone the repository (if not already cloned)
       ```bash
    git clone https://github.com/Nayekah/Tucil2_13523027_13523090.git
    ```
 3. Make Tucil2_13523027_13523090 as root directory:
       ```bash
    cd /Tucil2_13523027_13523090
    ```
 4. Install the dependencies first (if not already installed)
 5. Do: 
    ```bash
    build.bat

    # or

    \.build.bat
    ```

 #### 🧹 Clean
 1. Open a terminal
 2. Clone the repository (if not already cloned)
       ```bash
    git clone https://github.com/Nayekah/Tucil2_13523027_13523090.git
    ```
 3. Make Tucil2_13523027_13523090 as root directory:
       ```bash
    cd /Tucil2_13523027_13523090
    ```
 4. Do: 
    ```bash
    clean.bat

    # or

    \.clean.bat
    ```
 <br/>
 <br/>
 ---
 ### **🐧 Linux (UNIX system)**
 ### 🤖 Bots
 1. Open a terminal
 2. Clone the repository (if not already cloned)
       ```bash
    git clone https://github.com/zirachw/Tubes1_Rudal-Sekeloa-Reloaded.git
    ```
 3. Make Tubes1_Rudal-Sekeloa-Reloaded as root directory:
       ```bash
    cd Tubes1_Rudal-Sekeloa-Reloaded
    ```
 4. Direct it to bot's root folder:
    ```bash
    cd src/alternative-bots/[botname]
    ```
    if you want to compile alternative bots, or
       ```bash
    cd src/main-bots/[botname]
    ```
    if you want to compile main bot
 4. Compile the bot (bin obj making process):
    ```bash
    ./[botname].sh
    ```
 <br/>
 <br/>
 
 ### ⚙️ Game Engine
 (See the bots section first for the prerequisite)
 
 1. Open a terminal
 2. Clone the repository (if not already cloned)
       ```bash
    git clone https://github.com/zirachw/Tubes1_Rudal-Sekeloa-Reloaded.git
    ```
 3. Make Tubes1_Rudal-Sekeloa-Reloaded as root directory:
       ```bash
    cd Tubes1_Rudal-Sekeloa-Reloaded
    ```
 4. Run the following command to start the Game Engine
    ```bash
    java -jar robocode-tankroyale-gui-0.30.0.jar
    ```
 ---
 ## ✨ How to Run
![tutorial](https://github.com/user-attachments/assets/e428faf9-d83b-4160-b81a-e57a6c5aa4fe)

### Note: you can change the game rule in battle->Setup Rules.
 ---
 ## 📱 Repository Structure
 ```
 📂 Tubes1_Rudal-Sekeloa-Reloaded/
 ├── 📂 docs/
 │ ├── .gitkeep
 │ └── Rudal Sekeloa Reloaded.pdf
 ├── 📂 src/
 │ ├── 📂 alternative-bots/
 │ │ ├── 📂 Kaze/
 │ │ │ ├── Kaze.cmd
 │ │ │ ├── Kaze.cs
 │ │ │ ├── Kaze.csproj
 │ │ │ ├── Kaze.json
 │ │ │ └── Kaze.sh
 │ │ ├── 📂 Sweepredict/
 │ │ │ ├── Sweepredict.cmd
 │ │ │ ├── Sweepredict.cs
 │ │ │ ├── Sweepredict.csproj
 │ │ │ ├── Sweepredict.json
 │ │ │ └── Sweepredict.sh
 │ │ ├── 📂 Waves/
 │ │ │ ├── Waves.cmd
 │ │ │ ├── Waves.cs
 │ │ │ ├── Waves.csproj
 │ │ │ ├── Waves.json
 │ │ │ └── Waves.sh
 │ │ └── .gitkeep
 │ └── 📂 main-bots/
 │ │ ├── 📂 RudalSekeloa/
 │ │ │ ├── RudalSekeloa.cmd
 │ │ │ ├── RudalSekeloa.cs
 │ │ │ ├── RudalSekeloa.csproj
 │ │ │ ├── RudalSekeloa.json
 │ │ │ └── RudalSekeloa.sh
 │ │ └── .gitkeep
 ├──  README.md
 └──  robocode-tankroyale-gui-0.30.0.jar
 ```
 
 ---
 ## 📃 Miscellaneous
 <div align="center">
 
 | No | Points | Yes | No |
 | --- | --- | --- | --- |
 | 1 | The bot can be run on the Engine that the assistant has modified. | ✔️ | |
 | 2 | Created 4 greedy solutions with different heuristics. | ✔️ | |
 | 3 | Make reports according to specifications. | ✔️ | |
 | 4 | Created a bonus video and uploaded it on Youtube. | ✔️ | |
 
 </div>
 
 <br/>
 <br/>
 <br/>
 <br/>
 
 <div align="center">
 Rudal Sekeloa Reloaded • © 2025 • 13523004-13523090-13523098
 </div>
 
 <!-- MARKDOWN LINKS & IMAGES -->
 [Csharp-url]: https://learn.microsoft.com/en-us/dotnet/csharp/
 [Java-url]: https://www.java.com/en/
 [Gradle-url]: https://gradle.org/
