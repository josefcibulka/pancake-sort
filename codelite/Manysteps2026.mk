##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=Manysteps2026
ConfigurationName      :=Release
WorkspaceConfiguration :=Release
WorkspacePath          :=/home/pepa/Prgani/Pancakes/Manysteps2026
ProjectPath            :=/home/pepa/Prgani/Pancakes/Manysteps2026
IntermediateDirectory  :=$(ConfigurationName)
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Josef Cibulka
Date                   :=08/01/2026
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
OutputFile             :=$(IntermediateDirectory)/manysteps-unb
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="Manysteps2026.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  -O2 -std=gnu++20
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch). $(LibraryPathSwitch)Release 

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
Objects0=$(IntermediateDirectory)/StackLog.cpp$(ObjectSuffix) $(IntermediateDirectory)/MixedStack.cpp$(ObjectSuffix) $(IntermediateDirectory)/BucketedQueue.cpp$(ObjectSuffix) $(IntermediateDirectory)/Stack.cpp$(ObjectSuffix) $(IntermediateDirectory)/manysteps-unb.cc$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@test -d $(ConfigurationName) || $(MakeDirCommand) $(ConfigurationName)


$(IntermediateDirectory)/.d:
	@test -d $(ConfigurationName) || $(MakeDirCommand) $(ConfigurationName)

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/StackLog.cpp$(ObjectSuffix): StackLog.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/StackLog.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/StackLog.cpp$(DependSuffix) -MM StackLog.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pepa/Prgani/Pancakes/Manysteps2026/StackLog.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/StackLog.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/StackLog.cpp$(PreprocessSuffix): StackLog.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/StackLog.cpp$(PreprocessSuffix) StackLog.cpp

$(IntermediateDirectory)/MixedStack.cpp$(ObjectSuffix): MixedStack.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/MixedStack.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/MixedStack.cpp$(DependSuffix) -MM MixedStack.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pepa/Prgani/Pancakes/Manysteps2026/MixedStack.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/MixedStack.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/MixedStack.cpp$(PreprocessSuffix): MixedStack.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/MixedStack.cpp$(PreprocessSuffix) MixedStack.cpp

$(IntermediateDirectory)/BucketedQueue.cpp$(ObjectSuffix): BucketedQueue.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/BucketedQueue.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/BucketedQueue.cpp$(DependSuffix) -MM BucketedQueue.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pepa/Prgani/Pancakes/Manysteps2026/BucketedQueue.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/BucketedQueue.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/BucketedQueue.cpp$(PreprocessSuffix): BucketedQueue.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/BucketedQueue.cpp$(PreprocessSuffix) BucketedQueue.cpp

$(IntermediateDirectory)/Stack.cpp$(ObjectSuffix): Stack.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Stack.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Stack.cpp$(DependSuffix) -MM Stack.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pepa/Prgani/Pancakes/Manysteps2026/Stack.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Stack.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Stack.cpp$(PreprocessSuffix): Stack.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Stack.cpp$(PreprocessSuffix) Stack.cpp

$(IntermediateDirectory)/manysteps-unb.cc$(ObjectSuffix): manysteps-unb.cc
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/manysteps-unb.cc$(ObjectSuffix) -MF$(IntermediateDirectory)/manysteps-unb.cc$(DependSuffix) -MM manysteps-unb.cc
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pepa/Prgani/Pancakes/Manysteps2026/manysteps-unb.cc" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/manysteps-unb.cc$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/manysteps-unb.cc$(PreprocessSuffix): manysteps-unb.cc
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/manysteps-unb.cc$(PreprocessSuffix) manysteps-unb.cc


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r $(ConfigurationName)/


