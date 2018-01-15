[1mdiff --git a/External/veclib b/External/veclib[m
[1m--- a/External/veclib[m
[1m+++ b/External/veclib[m
[36m@@ -1 +1 @@[m
[31m-Subproject commit 0bd0e7b369b7d6781e1acf0b1ad1cef3b68ca691[m
[32m+[m[32mSubproject commit 0bd0e7b369b7d6781e1acf0b1ad1cef3b68ca691-dirty[m
[1mdiff --git a/Projects/WindowsPlayer/WindowsPlayer/WindowsPlayer.vcxproj b/Projects/WindowsPlayer/WindowsPlayer/WindowsPlayer.vcxproj[m
[1mindex 07f8b75..d9df23d 100644[m
[1m--- a/Projects/WindowsPlayer/WindowsPlayer/WindowsPlayer.vcxproj[m
[1m+++ b/Projects/WindowsPlayer/WindowsPlayer/WindowsPlayer.vcxproj[m
[36m@@ -91,7 +91,7 @@[m
       <WarningLevel>Level3</WarningLevel>[m
       <Optimization>Disabled</Optimization>[m
       <SDLCheck>true</SDLCheck>[m
[31m-      <PreprocessorDefinitions>__x86_64;DEBUG;%(PreprocessorDefinitions)</PreprocessorDefinitions>[m
[32m+[m[32m      <PreprocessorDefinitions>__x86_64;DEBUG;PLATFORM_WINDOWS;%(PreprocessorDefinitions)</PreprocessorDefinitions>[m
     </ClCompile>[m
     <Manifest>[m
       <EnableDpiAwareness>PerMonitorHighDPIAware</EnableDpiAwareness>[m
[36m@@ -117,7 +117,7 @@[m
       <FunctionLevelLinking>true</FunctionLevelLinking>[m
       <IntrinsicFunctions>true</IntrinsicFunctions>[m
       <SDLCheck>true</SDLCheck>[m
[31m-      <PreprocessorDefinitions>__x86_64;%(PreprocessorDefinitions)</PreprocessorDefinitions>[m
[32m+[m[32m      <PreprocessorDefinitions>__x86_64;PLATFORM_WINDOWS;%(PreprocessorDefinitions)</PreprocessorDefinitions>[m
     </ClCompile>[m
     <Link>[m
       <EnableCOMDATFolding>true</EnableCOMDATFolding>[m
[1mdiff --git a/Source/Foundation/ProfilerModule.cpp b/Source/Foundation/ProfilerModule.cpp[m
[1mindex 58f1cb7..f7c0b28 100644[m
[1m--- a/Source/Foundation/ProfilerModule.cpp[m
[1m+++ b/Source/Foundation/ProfilerModule.cpp[m
[36m@@ -10,5 +10,4 @@[m [mvoid ProfilerModule::SetupExecuteOrder(ModuleManager* moduleManager)[m
 [m
 void ProfilerModule::Execute(const ExecutionContext& context)[m
 {[m
[31m-	PROFILE_FUNCTION;[m
 }[m
[1mdiff --git a/Source/Foundation/TimeModule.cpp b/Source/Foundation/TimeModule.cpp[m
[1mindex 3f5750f..7ac56ec 100644[m
[1m--- a/Source/Foundation/TimeModule.cpp[m
[1m+++ b/Source/Foundation/TimeModule.cpp[m
[36m@@ -1,33 +1,19 @@[m
 #include <Foundation\TimeModule.h>[m
[31m-#include <Foundation\LogModule.h>[m
 #include <Tools\Math\Math.h>[m
 [m
 TimeModule::TimeModule() [m
[31m-	: passedFrameCount(0)[m
 {[m
[31m-	stopWatch.Start();[m
 }[m
 [m
 void TimeModule::SetupExecuteOrder(ModuleManager* moduleManager)[m
 {[m
 	Module::SetupExecuteOrder(moduleManager);[m
[31m-	logModule = ExecuteBefore<LogModule>(moduleManager);[m
[32m+[m	[32mstopWatch.Start();[m
 }[m
 [m
 void TimeModule::Execute(const ExecutionContext& context)[m
 {[m
[31m-	PROFILE_FUNCTION;[m
 	stopWatch.Stop();[m
[31m-	if (stopWatch.GetElapsedMiliseconds() >= 8000)[m
[31m-	{[m
[31m-		// 78 58 59[m
[31m-		// 69 59 61[m
[31m-		// 67 57 57[m
[31m-		// 36 33 33[m
[31m-		TRACE("Frame took ms %f", (float) stopWatch.GetElapsedMiliseconds() / passedFrameCount);[m
[31m-		logModule->RecWriteFmt(context, "Frame took ms %f\n", (float) stopWatch.GetElapsedMiliseconds() / passedFrameCount);[m
[31m-		stopWatch.Start();[m
[31m-		passedFrameCount = 0;[m
[31m-	}[m
[31m-	passedFrameCount++;[m
[31m-}[m
[32m+[m	[32mdeltaTimeMs = stopWatch.GetElapsedMiliseconds();[m
[32m+[m	[32mstopWatch.Start();[m
[32m+[m[32m}[m
\ No newline at end of file[m
[1mdiff --git a/Source/Foundation/TimeModule.h b/Source/Foundation/TimeModule.h[m
[1mindex 23b310c..7f0fc27 100644[m
[1m--- a/Source/Foundation/TimeModule.h[m
[1m+++ b/Source/Foundation/TimeModule.h[m
[36m@@ -12,9 +12,9 @@[m [mpublic:[m
 	TimeModule();[m
 	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;[m
 	virtual void Execute(const ExecutionContext& context) override;[m
[32m+[m	[32minline uint64_t GetDeltaTimeMs() const { return stopWatch.GetElapsedMiliseconds(); }[m
 [m
 private:[m
[31m-	LogModule* logModule;[m
 	StopWatch stopWatch;[m
[31m-	uint64_t passedFrameCount;[m
[32m+[m	[32muint64_t deltaTimeMs;[m
 };[m
\ No newline at end of file[m
[1mdiff --git a/Source/Modules/Profiler.cpp b/Source/Modules/Profiler.cpp[m
[1mindex 05211e7..6ce4aeb 100644[m
[1m--- a/Source/Modules/Profiler.cpp[m
[1m+++ b/Source/Modules/Profiler.cpp[m
[36m@@ -5,6 +5,7 @@[m [mProfiler::Profiler(uint32_t workersCount)[m
 	: workers(workersCount)[m
 {[m
 	Reset();[m
[32m+[m	[32mstopWatch.Start();[m
 }[m
 [m
 void Profiler::StartFunction([m
[36m@@ -20,7 +21,7 @@[m [mvoid Profiler::StartFunction([m
 	function.functionName = functionName;[m
 	function.lineNumber = lineNumber;[m
 	function.parent = worker.currentFunction;[m
[31m-	function.startCycle = Cpu::GetCycles();[m
[32m+[m	[32mfunction.startCycle = stopWatch.GetElapsedPicoseconds();[m
 [m
 	worker.functions.push_back(function);[m
 	worker.currentFunction = &worker.functions.back();[m
[36m@@ -30,7 +31,7 @@[m [mvoid Profiler::StopFunction(uint32_t workerIndex)[m
 {[m
 	auto& worker = workers[workerIndex];[m
 	ASSERT(worker.currentFunction != nullptr);[m
[31m-	worker.currentFunction->endCycle = Cpu::GetCycles();[m
[32m+[m	[32mworker.currentFunction->endCycle = stopWatch.GetElapsedPicoseconds();[m
 	worker.currentFunction = worker.currentFunction->parent;[m
 }[m
 [m
[1mdiff --git a/Source/Modules/Profiler.h b/Source/Modules/Profiler.h[m
[1mindex 717b3f3..3c19745 100644[m
[1m--- a/Source/Modules/Profiler.h[m
[1m+++ b/Source/Modules/Profiler.h[m
[36m@@ -1,6 +1,7 @@[m
 #pragma once[m
 [m
 #include <Tools\Common.h>[m
[32m+[m[32m#include <Tools\StopWatch.h>[m
 #include <Modules\IProfiler.h>[m
 [m
 class Profiler : public IProfiler[m
[36m@@ -19,4 +20,5 @@[m [mpublic:[m
 [m
 private:[m
 	List<Worker> workers;[m
[32m+[m	[32mStopWatch stopWatch;[m
 };[m
\ No newline at end of file[m
[1mdiff --git a/Source/Tools/Console.h b/Source/Tools/Console.h[m
[1mindex 16f8aaf..6e49f5f 100644[m
[1m--- a/Source/Tools/Console.h[m
[1m+++ b/Source/Tools/Console.h[m
[36m@@ -3,7 +3,7 @@[m
 #include <stdio.h>[m
 #include <cstdarg>[m
 [m
[31m-#if _WIN32[m
[32m+[m[32m#if PLATFORM_WINDOWS[m
 #	ifndef WIN32_LEAN_AND_MEAN[m
 #		define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers.[m
 #	endif[m
[36m@@ -19,7 +19,7 @@[m [mnamespace Console[m
 {[m
 	inline void Write(const char* msg) [m
 	{ [m
[31m-#if _WIN32[m
[32m+[m[32m#if PLATFORM_WINDOWS[m
 		OutputDebugString(msg);[m
 #else[m
 		ERROR("Not implemented");[m
[1mdiff --git a/Source/Tools/Cpu.h b/Source/Tools/Cpu.h[m
[1mindex be62d80..3a68ba1 100644[m
[1m--- a/Source/Tools/Cpu.h[m
[1m+++ b/Source/Tools/Cpu.h[m
[36m@@ -2,7 +2,7 @@[m
 [m
 #include <Tools\Common.h>[m
 [m
[31m-#if _WIN32[m
[32m+[m[32m#if PLATFORM_WINDOWS[m
 #include <Windows.h>[m
 #endif[m
 [m
[36m@@ -10,9 +10,13 @@[m [mnamespace Cpu[m
 {[m
 	uint64_t GetCycles()[m
 	{[m
[32m+[m[32m#if PLATFORM_WINDOWS[m
 		FILETIME ftime, fsys, fuser;[m
 		GetProcessTimes(GetCurrentProcess(), &ftime, &ftime, &fsys, &fuser);[m
 		return fuser.dwLowDateTime;[m
[32m+[m[32m#else[m
[32m+[m		[32mERROR("Not implemented");[m
[32m+[m[32m#endif[m
 	}[m
 [m
 	uint32_t GetCoreCount()[m
[1mdiff --git a/Source/Tools/IO/Directory.h b/Source/Tools/IO/Directory.h[m
[1mindex f2ad501..fa87151 100644[m
[1m--- a/Source/Tools/IO/Directory.h[m
[1m+++ b/Source/Tools/IO/Directory.h[m
[36m@@ -1,7 +1,7 @@[m
 #pragma once[m
 [m
 #include <Tools\Common.h>[m
[31m-#if _WIN32[m
[32m+[m[32m#if PLATFORM_WINDOWS[m
 #include <Windows.h>[m
 #endif[m
 [m
[36m@@ -13,7 +13,7 @@[m [mnamespace Directory[m
 		static bool pathValid = false;[m
 [m
 		// Store executable path on demand[m
[31m-#if _WIN32[m
[32m+[m[32m#if PLATFORM_WINDOWS[m
 		if (!pathValid)[m
 		{[m
 			HMODULE hModule = GetModuleHandleW(NULL);[m
[1mdiff --git a/Source/Tools/StopWatch.h b/Source/Tools/StopWatch.h[m
[1mindex 3cbc0ba..bd8418b 100644[m
[1m--- a/Source/Tools/StopWatch.h[m
[1m+++ b/Source/Tools/StopWatch.h[m
[36m@@ -22,13 +22,13 @@[m [mpublic:[m
 		end = clock::now();[m
 	}[m
 [m
[31m-	inline uint64_t GetElapsedPicoseconds()[m
[32m+[m	[32minline uint64_t GetElapsedPicoseconds() const[m
 	{[m
 		auto ticks_per_iter = Cycle(end - start) / 1;[m
 		return std::chrono::duration_cast<picoseconds>(ticks_per_iter).count();[m
 	}[m
 [m
[31m-	inline uint64_t GetElapsedMiliseconds()[m
[32m+[m	[32minline uint64_t GetElapsedMiliseconds() const[m
 	{[m
 		auto ticks_per_iter = Cycle(end - start) / 1;[m
 		return std::chrono::duration_cast<milliseconds>(ticks_per_iter).count();[m
[1mdiff --git a/Source/WindowsPlayer/Main.cpp b/Source/WindowsPlayer/Main.cpp[m
[1mindex fdfe72f..924b74b 100644[m
[1m--- a/Source/WindowsPlayer/Main.cpp[m
[1m+++ b/Source/WindowsPlayer/Main.cpp[m
[36m@@ -25,6 +25,49 @@[m
 #include <Windows\Graphics\D12\D12GraphicsModule.h>[m
 #include <Windows\Views\WinViewModule.h>[m
 [m
[32m+[m[32mclass FpsLoggerModule : public Module[m
[32m+[m[32m{[m
[32m+[m[32mpublic:[m
[32m+[m	[32mFpsLoggerModule()[m
[32m+[m		[32m: passedFrameCount(0)[m
[32m+[m	[32m{[m
[32m+[m	[32m}[m
[32m+[m
[32m+[m	[32mvirtual void SetupExecuteOrder(ModuleManager* moduleManager) override[m
[32m+[m	[32m{[m
[32m+[m		[32mModule::SetupExecuteOrder(moduleManager);[m
[32m+[m		[32mlogModule = ExecuteBefore<LogModule>(moduleManager);[m
[32m+[m		[32mtimeModule = ExecuteAfter<TimeModule>(moduleManager);[m
[32m+[m	[32m}[m
[32m+[m
[32m+[m	[32mvirtual void Execute(const ExecutionContext& context) override[m
[32m+[m	[32m{[m
[32m+[m		[32mPROFILE_FUNCTION;[m
[32m+[m		[32mauto dt = timeModule->GetDeltaTimeMs();[m
[32m+[m		[32mif (timePassed >= 8000)[m
[32m+[m		[32m{[m
[32m+[m			[32m// 78 58 59[m
[32m+[m			[32m// 69 59 61[m
[32m+[m			[32m// 67 57 57[m
[32m+[m			[32m// 36 33 33[m
[32m+[m			[32mauto msPerFrame = (float) dt / passedFrameCount;[m
[32m+[m			[32mTRACE("Frame took ms %f", msPerFrame);[m
[32m+[m			[32mlogModule->RecWriteFmt(context, "Frame took ms %f\n", msPerFrame);[m
[32m+[m
[32m+[m			[32mpassedFrameCount = 0;[m
[32m+[m			[32mtimePassed = 0;[m
[32m+[m		[32m}[m
[32m+[m		[32mpassedFrameCount++;[m
[32m+[m		[32mtimePassed += dt;[m
[32m+[m	[32m}[m
[32m+[m
[32m+[m[32mprivate:[m
[32m+[m	[32mLogModule* logModule;[m
[32m+[m	[32mTimeModule* timeModule;[m
[32m+[m	[32muint64_t passedFrameCount;[m
[32m+[m	[32muint64_t timePassed;[m
[32m+[m[32m};[m
[32m+[m
 class TestModule : public Module[m
 {[m
 public:[m
[36m@@ -251,6 +294,7 @@[m [mint WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)[m
 	auto executer = new ConcurrentModuleExecuter(planner, 4);[m
 	auto moduleManager = new ModuleManager(planner, executer, new Profiler(4));[m
 [m
[32m+[m	[32m// Core[m
 	moduleManager->AddModule(new LogModule());[m
 	moduleManager->AddModule(new UnitModule());[m
 	moduleManager->AddModule(new D12GraphicsModule());[m
[36m@@ -266,10 +310,13 @@[m [mint WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)[m
 	moduleManager->AddModule(new SurfaceModule());[m
 	moduleManager->AddModule(new CameraModule());[m
 	moduleManager->AddModule(new UnlitRenderingLoopModule());[m
[31m-	moduleManager->AddModule(new TestModule());[m
 	moduleManager->AddModule(new TimeModule());[m
 	moduleManager->AddModule(new MemoryModule());[m
 [m
[32m+[m	[32m// Test project[m
[32m+[m	[32mmoduleManager->AddModule(new TestModule());[m
[32m+[m	[32mmoduleManager->AddModule(new FpsLoggerModule());[m
[32m+[m
 	moduleManager->Start();[m
 	while (moduleManager->IsRunning())[m
 	{[m
