#pragma once


namespace Asciir
{
	// Define this function in your program and return a custom class derived from Asciir::AREngine
	extern AREngine* createEngine(std::vector<std::string> args);
}

int main(int argc, char** argv)
{
	bool append_logs = false;
	bool save_core = true;
	bool save_client = true;

#ifdef AR_LOG_APPEND
	append_logs = true;
#endif
#ifdef AR_CORE_LOG_DELETE
	save_core = false;
#endif
#ifdef AR_CLEINT_LOG_DELETE
	save_core = false;
#endif

	Asciir::Log::init(save_core, save_client, append_logs);

#if defined(AR_WIN)
	Asciir::WindowsInit();
#elif defined(AR_LINUX)
	Asciir::LinuxInit();
#elif defined(AR_MAC)
	Asciir::MacInit();
#endif

	std::vector<std::string> args(argc);
	for (int i = 0; i < argc; i++)
	{
		args[i] = std::move(argv[i]);
	}

	Asciir::AREngine::create(Asciir::createEngine(std::move(args)));

	Asciir::Renderer::init();

	Asciir::AREngine::getEngine()->start();
	delete Asciir::AREngine::getEngine();
}
