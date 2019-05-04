# Secure-Voice-Calls
Secure Voice Calls project. Owners Andrii Nevidomii and Max Nesterchuk

Requirements:
IDE - QtCreator 
cmake version 3.1+
qt5: qcore qtquick
conan package manager (grpc/1.20.0@inexorgame/stable)


Build steps:
1) install grpc package in conan:
conan remote add inexorgame "https://api.bintray.com/conan/inexorgame/nexor-conan"
conan install grpc/1.20.0@inexorgame/stable  // REQUIRED grpc
	
cd to ~/.conan/data/protobuf/3.6.1/bincrafters/stable/build/56e0cf6d16ee57367a0661ab743f4e43b29223f8/source_subfolder/src
	and follow installing instruction in ReadMe file

2) create build directory for client/server projects

3) add conanfile.txt to source directory with content:
[requires]
grpc/1.20.0@inexorgame/stable

[generators]
cmake

4) in build directory execute command:
conan install <PATH TO conanfile.txt>

5) open CMakeLists.txt by QtCreator(additionaly configure cmake if didn`t)
build ->run cmake
build ->build and run


