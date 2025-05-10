#include <memory>
#include "CSOL_Utilities.hpp"
#include "Command.hpp"
#include "CommandDispatcher.hpp"
#include "Event.hpp"
#include "GameProcessDetector.hpp"
#include "OCRIdleEngine.hpp"
#include "OcrIdleEngine.hpp"
#include "Signal.hpp"

namespace CSOL_Utilities
{
	void Bootstrap()
	{
		/* main */
		// main -> process detector
		auto signal_Main_ProcessDetector = Signal::create();
		auto out_Main_ProcessDetector = LogicElementFactory::MakePin(signal_Main_ProcessDetector);
		out_Main_ProcessDetector->acquire();
		// main -> idle engine
		auto signal_Main_IdleEngine = Signal::create();
		auto out_Main_IdleEngine = LogicElementFactory::MakePin(signal_Main_IdleEngine);
		out_Main_IdleEngine->acquire();
		// main -> command dispatcher
		auto signal_Main_CommandDispatcher = Signal::create();
		auto out_Main_CommandDispatcher = LogicElementFactory::MakePin(signal_Main_CommandDispatcher);
		out_Main_CommandDispatcher->acquire();
		/* process detector */
		// working condition: main -> process detector
		auto in_ProcessDetector = LogicElementFactory::MakePin(signal_Main_ProcessDetector);
		// process detector -> idle engine
		auto signal_ProcessDetector_IdleEngine = Signal::create();
		auto out_ProcessDetector_IdleEngine = LogicElementFactory::MakePin(signal_ProcessDetector_IdleEngine);
		out_ProcessDetector_IdleEngine->acquire();
		/* idle engine */
		// working condition: main -> idle engine && process detector -> idle engine
		auto andin_IdleEngine =
			LogicElementFactory::MakeAndGate(signal_Main_IdleEngine, signal_ProcessDetector_IdleEngine);
		// idle engine -> command dispatcher
		auto signal_IdleEngine_CommandDispatcher = Signal::create();
		auto out_IdleEngine_CommandDispatcher = LogicElementFactory::MakePin(signal_IdleEngine_CommandDispatcher);
		out_IdleEngine_CommandDispatcher->acquire();
		/* command dispatcher */
		// working condition: main -> command dispatcher || idle engine -> command dispatcher
		auto orin_CommandDispatcher =
			LogicElementFactory::MakeOrGate(signal_Main_CommandDispatcher, signal_IdleEngine_CommandDispatcher);

		auto module_exit_event = std::make_shared<Event>();

		GameProcessDetector game_process_detector(L"cstrike-online.exe", ConvertUtf8ToUtf16(Opt::g_LaunchGameCmd));
		game_process_detector.input(in_ProcessDetector);
		game_process_detector.output(Opt::g_GameProcessDetectorName, out_ProcessDetector_IdleEngine);
		game_process_detector.bootstrap();

		auto cmd = std::make_shared<Command>();

		OCRIdleEngine ocr_idle_engine(Opt::g_OCRKeywordsPath, Opt::g_OCRDetectionModelPath,
									  Opt::g_OCRRecognitionModelPath, Opt::g_OCRDictionaryPath, 0, cmd);
		ocr_idle_engine.input(andin_IdleEngine);
		ocr_idle_engine.output(Opt::g_IdleEngineName, out_IdleEngine_CommandDispatcher);
		ocr_idle_engine.bootstrap();

		CommandDispatcher command_dispatcher;
		command_dispatcher.input(orin_CommandDispatcher);
		command_dispatcher.bootstrap();
	}
} // namespace CSOL_Utilities
