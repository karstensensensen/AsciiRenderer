#pragma once

#include "Terminal.h"
#include "Asciir/Event/KeyEvent.h"
#include "Asciir/Event/MouseEvent.h"
#include "Asciir/Event/TerminalEvent.h"
#include "LayerStack.h"
#include <ETH.h>

namespace Asciir
{
	class AREngine
	{
	protected:
		AREngine();
	public:
		virtual ~AREngine();

		virtual void start();

		void run();

		void render();

		void onEvent(Event& event);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		static void create(AREngine* engine);

		static AREngine* const getEngine();
		Terminal& getTerminal();

	private:

		bool onTerminalClose(TerminalClosedEvent& e);

		Terminal m_terminal;
		bool m_running = false;
		LayerStack m_layerStack;
		duration m_last_frame_start;
		duration m_engine_start;
		size_t m_frame_count = 0;

		// thread gets started every time an update is finished, unless a thread already is running
		ETH::LThread m_render_thread;

		static AREngine* i_engine;
	};

	AREngine* createEngine(std::vector<std::string> args);
}
