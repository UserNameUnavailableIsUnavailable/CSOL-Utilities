#include "pch.hpp"

#include "OCR.hpp"
#include "Utilities.hpp"

constexpr const char* DETECTION_MODEL_FILE = "models/ch_PP-OCRv4_det_infer.onnx";
// constexpr const char* CLS_MODEL_FILE = "models/ch_ppocr_mobile_v2.0_cls_infer_model.onnx";
constexpr const char* RECOGNITION_MODEL_FILE = "models/ch_PP-OCRv4_rec_infer.onnx";
constexpr const char* DICTIONARY_FILE = "models/dictionary.txt";

#ifdef Test__
int main()
{
	using namespace CSOL_Utilities;
	std::vector<uint8_t> buffer;
	CaptureWindowAsBmp(nullptr, buffer);
	cv::Mat mat = cv::imdecode(buffer, cv::IMREAD_COLOR);
	OCR ocr(DETECTION_MODEL_FILE, RECOGNITION_MODEL_FILE, DICTIONARY_FILE, 2);
	OCRParam param{
		.nPadding = 50, .nMaxSideLength = 1024, .fBoxScoreThreshold = 0.6f, .fBoxThreshold = 0.3f, .fUnclipRatio = 2.0f,
	};
	auto result = ocr.Detect(mat, param);
	std::cout << result.content << std::endl;
}

//int main()
//{
//	using namespace CSOL_Utilities;
//	std::vector<uint8_t> buffer;
//	CaptureWindowAsBmp(nullptr, buffer);
//	std::ofstream ofs("$capture.bmp", std::ios::binary | std::ios::out);
//	ofs.write(reinterpret_cast<char*>(buffer.data()), buffer.size());
//}
#endif
