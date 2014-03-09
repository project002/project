################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/CConnection.cpp \
../src/CDHCPService.cpp \
../src/CEmulation.cpp \
../src/CException.cpp \
../src/CGUIGTK.cpp \
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
./src/CGUIGTK.o \
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
./src/CGUIGTK.d \
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
	g++ -O2 -g -Wall -c -fmessage-length=0 -lcrafter -lboost_thread -pthread -I/usr/include/gtkmm-3.0 -I/usr/lib/i386-linux-gnu/gtkmm-3.0/include -I/usr/include/giomm-2.4 -I/usr/lib/i386-linux-gnu/giomm-2.4/include -I/usr/include/gtk-3.0 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/gtk-3.0/unix-print -I/usr/include/gdkmm-3.0 -I/usr/lib/i386-linux-gnu/gdkmm-3.0/include -I/usr/include/atk-1.0 -I/usr/include/glibmm-2.4 -I/usr/lib/i386-linux-gnu/glibmm-2.4/include -I/usr/include/glib-2.0 -I/usr/lib/i386-linux-gnu/glib-2.0/include -I/usr/include/sigc++-2.0 -I/usr/lib/i386-linux-gnu/sigc++-2.0/include -I/usr/include/pango-1.0 -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng12 -I/usr/include/gio-unix-2.0/ -I/usr/include/atkmm-1.6 -I/usr/include/pangomm-1.4 -I/usr/lib/pangomm-1.4/include -I/usr/include/cairomm-1.0 -I/usr/lib/cairomm-1.0/include  -lgtkmm-3.0 -latkmm-1.6 -lgdkmm-3.0 -lgiomm-2.4 -lpangomm-1.4 -lgtk-3 -lglibmm-2.4 -lcairomm-1.0 -lgdk-3 -latk-1.0 -lgio-2.0 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lcairo-gobject -lpango-1.0 -lcairo -lsigc-2.0 -lgobject-2.0 -lglib-2.0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Emulation\ Project.o: ../src/Emulation\ Project.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -lcrafter -lboost_thread -pthread -I/usr/include/gtkmm-3.0 -I/usr/lib/i386-linux-gnu/gtkmm-3.0/include -I/usr/include/giomm-2.4 -I/usr/lib/i386-linux-gnu/giomm-2.4/include -I/usr/include/gtk-3.0 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/gtk-3.0/unix-print -I/usr/include/gdkmm-3.0 -I/usr/lib/i386-linux-gnu/gdkmm-3.0/include -I/usr/include/atk-1.0 -I/usr/include/glibmm-2.4 -I/usr/lib/i386-linux-gnu/glibmm-2.4/include -I/usr/include/glib-2.0 -I/usr/lib/i386-linux-gnu/glib-2.0/include -I/usr/include/sigc++-2.0 -I/usr/lib/i386-linux-gnu/sigc++-2.0/include -I/usr/include/pango-1.0 -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng12 -I/usr/include/gio-unix-2.0/ -I/usr/include/atkmm-1.6 -I/usr/include/pangomm-1.4 -I/usr/lib/pangomm-1.4/include -I/usr/include/cairomm-1.0 -I/usr/lib/cairomm-1.0/include  -lgtkmm-3.0 -latkmm-1.6 -lgdkmm-3.0 -lgiomm-2.4 -lpangomm-1.4 -lgtk-3 -lglibmm-2.4 -lcairomm-1.0 -lgdk-3 -latk-1.0 -lgio-2.0 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lcairo-gobject -lpango-1.0 -lcairo -lsigc-2.0 -lgobject-2.0 -lglib-2.0 -MMD -MP -MF"src/Emulation Project.d" -MT"src/Emulation\ Project.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


