################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/CBuffer.cpp \
../src/CEmulation.cpp \
../src/CException.cpp \
../src/CPacket.cpp \
../src/CPacketARP.cpp \
../src/CPacketCollector.cpp \
../src/CPacketIPv4.cpp \
../src/CPacketIPv6.cpp \
../src/CPhysicalConnection.cpp \
../src/CPhysicalConnectionsHandler.cpp \
../src/Emulation\ Project.cpp 

OBJS += \
./src/CBuffer.o \
./src/CEmulation.o \
./src/CException.o \
./src/CPacket.o \
./src/CPacketARP.o \
./src/CPacketCollector.o \
./src/CPacketIPv4.o \
./src/CPacketIPv6.o \
./src/CPhysicalConnection.o \
./src/CPhysicalConnectionsHandler.o \
./src/Emulation\ Project.o 

CPP_DEPS += \
./src/CBuffer.d \
./src/CEmulation.d \
./src/CException.d \
./src/CPacket.d \
./src/CPacketARP.d \
./src/CPacketCollector.d \
./src/CPacketIPv4.d \
./src/CPacketIPv6.d \
./src/CPhysicalConnection.d \
./src/CPhysicalConnectionsHandler.d \
./src/Emulation\ Project.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Emulation\ Project.o: ../src/Emulation\ Project.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/Emulation Project.d" -MT"src/Emulation\ Project.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


