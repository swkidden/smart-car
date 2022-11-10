################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/User/LQ_AnoScope.c \
../src/User/LQ_ImageProcess.c \
../src/User/LQ_Inductor.c \
../src/User/LQ_MotorServo.c \
../src/User/LQ_PID.c \
../src/User/balingyu.c \
../src/User/guaidian.c 

OBJS += \
./src/User/LQ_AnoScope.o \
./src/User/LQ_ImageProcess.o \
./src/User/LQ_Inductor.o \
./src/User/LQ_MotorServo.o \
./src/User/LQ_PID.o \
./src/User/balingyu.o \
./src/User/guaidian.o 

COMPILED_SRCS += \
./src/User/LQ_AnoScope.src \
./src/User/LQ_ImageProcess.src \
./src/User/LQ_Inductor.src \
./src/User/LQ_MotorServo.src \
./src/User/LQ_PID.src \
./src/User/balingyu.src \
./src/User/guaidian.src 

C_DEPS += \
./src/User/LQ_AnoScope.d \
./src/User/LQ_ImageProcess.d \
./src/User/LQ_Inductor.d \
./src/User/LQ_MotorServo.d \
./src/User/LQ_PID.d \
./src/User/balingyu.d \
./src/User/guaidian.d 


# Each subdirectory must supply rules for building sources it contributes
src/User/%.src: ../src/User/%.c src/User/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING C/C++ Compiler'
	cctc -D__CPU__=tc26xb "-fD:/program/AURIX/完赛二号-提速版/Debug/TASKING_C_C___Compiler-Include_paths.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -o "$@"  "$<"  -cs --dep-file="$(@:.src=.d)" --misrac-version=2012 -N0 -Z0 -Y0 2>&1;
	@echo 'Finished building: $<'
	@echo ' '

src/User/%.o: ./src/User/%.src src/User/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING Assembler'
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<" --list-format=L1 --optimize=gs
	@echo 'Finished building: $<'
	@echo ' '


