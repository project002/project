################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/CEmulation.cpp \
../src/CException.cpp \
../src/CPhysicalConnection.cpp \
../src/CPhysicalConnectionsHandler.cpp \
../src/Emulation\ Project.cpp 

OBJS += \
./src/CEmulation.o \
./src/CException.o \
./src/CPhysicalConnection.o \
./src/CPhysicalConnectionsHandler.o \
./src/Emulation\ Project.o 

CPP_DEPS += \
./src/CEmulation.d \
./src/CException.d \
./src/CPhysicalConnection.d \
./src/CPhysicalConnectionsHandler.d \
./src/Emulation\ Project.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Emulation\ Project.o: ../src/Emulation\ Project.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"src/Emulation Project.d" -MT"src/Emulation\ Project.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


