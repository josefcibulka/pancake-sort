##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=manysteps-lib
ConfigurationName      :=Release
WorkspaceConfiguration :=Release
WorkspacePath          :=/home/pepa/Prgani/Pancakes/Manysteps2026/codelite
ProjectPath            :=/home/pepa/Prgani/Pancakes/Manysteps2026/codelite
IntermediateDirectory  :=$(ConfigurationName)
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Josef Cibulka
Date                   :=08/27/2026
CodeLitePath           :=/home/pepa/.codelite
LinkerName             :=/usr/bin/g++
SharedObjectLinkerName :=/usr/bin/g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputDirectory        :=$(IntermediateDirectory)
OutputFile             :=$(IntermediateDirectory)/lib$(ProjectName).a
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="manysteps-lib.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overridden using an environment variable
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS :=  -O2 -W -Wall -std=gnu++20 $(Preprocessors)
CFLAGS   :=   $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/up_src_MixedStack.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_Stack.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_StackLog.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_BucketedQueue.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_BurntStack.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(IntermediateDirectory) $(OutputFile)

$(OutputFile): $(Objects)
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(AR) $(ArchiveOutputSwitch)$(OutputFile) @$(ObjectsFileList)
	@$(MakeDirCommand) "/home/pepa/Prgani/Pancakes/Manysteps2026/codelite/.build-release"
	@echo rebuilt > "/home/pepa/Prgani/Pancakes/Manysteps2026/codelite/.build-release/manysteps-lib"

MakeIntermediateDirs:
	@test -d $(ConfigurationName) || $(MakeDirCommand) $(ConfigurationName)


$(ConfigurationName):
	@test -d $(ConfigurationName) || $(MakeDirCommand) $(ConfigurationName)

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/up_src_MixedStack.cpp$(ObjectSuffix): ../src/MixedStack.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_MixedStack.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_MixedStack.cpp$(DependSuffix) -MM ../src/MixedStack.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pepa/Prgani/Pancakes/Manysteps2026/src/MixedStack.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_MixedStack.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_MixedStack.cpp$(PreprocessSuffix): ../src/MixedStack.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_MixedStack.cpp$(PreprocessSuffix) ../src/MixedStack.cpp

$(IntermediateDirectory)/up_src_Stack.cpp$(ObjectSuffix): ../src/Stack.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_Stack.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_Stack.cpp$(DependSuffix) -MM ../src/Stack.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pepa/Prgani/Pancakes/Manysteps2026/src/Stack.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_Stack.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_Stack.cpp$(PreprocessSuffix): ../src/Stack.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_Stack.cpp$(PreprocessSuffix) ../src/Stack.cpp

$(IntermediateDirectory)/up_src_StackLog.cpp$(ObjectSuffix): ../src/StackLog.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_StackLog.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_StackLog.cpp$(DependSuffix) -MM ../src/StackLog.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pepa/Prgani/Pancakes/Manysteps2026/src/StackLog.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_StackLog.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_StackLog.cpp$(PreprocessSuffix): ../src/StackLog.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_StackLog.cpp$(PreprocessSuffix) ../src/StackLog.cpp

$(IntermediateDirectory)/up_src_BucketedQueue.cpp$(ObjectSuffix): ../src/BucketedQueue.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_BucketedQueue.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_BucketedQueue.cpp$(DependSuffix) -MM ../src/BucketedQueue.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pepa/Prgani/Pancakes/Manysteps2026/src/BucketedQueue.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_BucketedQueue.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_BucketedQueue.cpp$(PreprocessSuffix): ../src/BucketedQueue.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_BucketedQueue.cpp$(PreprocessSuffix) ../src/BucketedQueue.cpp

$(IntermediateDirectory)/up_src_BurntStack.cpp$(ObjectSuffix): ../src/BurntStack.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_BurntStack.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_BurntStack.cpp$(DependSuffix) -MM ../src/BurntStack.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pepa/Prgani/Pancakes/Manysteps2026/src/BurntStack.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_BurntStack.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_BurntStack.cpp$(PreprocessSuffix): ../src/BurntStack.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_BurntStack.cpp$(PreprocessSuffix) ../src/BurntStack.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r $(ConfigurationName)/


