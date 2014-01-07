################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/CConnection.cpp \
../src/CDHCPService.cpp \
../src/CEmulation.cpp \
../src/CException.cpp \
../src/CPacketCollector.cpp \
../src/CPhysicalConnection.cpp \
../src/CPhysicalConnectionsHandler.cpp \
../src/CRouter.cpp \
../src/CVirtualConnection.cpp \
../src/Emulation\ Project.cpp \
../src/pugixml.cpp 

OBJS += \
./src/CConnection.o \
./src/CDHCPService.o \
./src/CEmulation.o \
./src/CException.o \
./src/CPacketCollector.o \
./src/CPhysicalConnection.o \
./src/CPhysicalConnectionsHandler.o \
./src/CRouter.o \
./src/CVirtualConnection.o \
./src/Emulation\ Project.o \
./src/pugixml.o 

CPP_DEPS += \
./src/CConnection.d \
./src/CDHCPService.d \
./src/CEmulation.d \
./src/CException.d \
./src/CPacketCollector.d \
./src/CPhysicalConnection.d \
./src/CPhysicalConnectionsHandler.d \
./src/CRouter.d \
./src/CVirtualConnection.d \
./src/Emulation\ Project.d \
./src/pugixml.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -lcrafter -lboost_thread-mt -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Emulation\ Project.o: ../src/Emulation\ Project.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -lcrafter -lboost_thread-mt -c -fmessage-length=0 -MMD -MP -MF"src/Emulation Project.d" -MT"src/Emulation\ Project.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


