FROM cirrusci/windowsservercore:cmake

RUN choco upgrade -y visualstudio2017community visualstudio2017-workload-vctools
RUN choco install -y visualstudio2017-workload-nativedesktop
RUN cinst -y -r make aria2 7zip

WORKDIR C:\\ 
#try trailing slash+space

RUN git clone https://github.com/Microsoft/vcpkg.git
RUN powershell -Command {cd vcpkg; .\bootstrap-vcpkg.bat; .\vcpkg integrate install}
RUN powershell -Command {.\vcpkg install libsndfile:x64-windows libflac:x64-windows libvorbis:x64-windows libogg:x64-windows spdlog:x64-windows boost-filesystem:x64-windows boost-program-options:x64-windows}
RUN powershell -Command {.\vcpkg install libsndfile:x86-windows libflac:x86-windows libvorbis:x86-windows libogg:x86-windows spdlog:x86-windows boost-filesystem:x86-windows boost-program-options:x86-windows}
