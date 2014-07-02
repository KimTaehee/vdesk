# MAKEFILE - Toby Opferman Copyright (c) 2005, All Rights Reserved

# "nmake"  can be used to build all directories
all:
   cd appmon
   nmake 
   cd ..
   cd gdi
   nmake 
   cd ..
   cd miniport
   nmake 
   cd ..

# "nmake clean"  can be used to clean up
clean:
   cd appmon
   nmake clean
   cd ..
   cd gdi
   nmake clean
   cd ..
   cd miniport
   nmake clean
   cd ..

# "nmake dirs /i"  can be used to create the directories
# and ignore errors if the "bin" directory is already created
dirs:
   mkdir ..\..\bin
   mkdir ..\..\bin\symbols