FROM cirrusci/windowsservercore:cmake

RUN choco upgrade -y visualstudio2017community visualstudio2017-workload-vctools
RUN choco install -y visualstudio2017-workload-nativedesktop
RUN cinst -y -r make aria2 7zip