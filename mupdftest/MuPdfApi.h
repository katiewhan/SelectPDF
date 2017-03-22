
#if _MSC_VER // this is defined when compiling with Visual Studio
#define EXPORT_API __declspec(dllexport) // Visual Studio needs annotating exported functions with this
#else
#define EXPORT_API // XCode does not need annotating exported functions, so define is empty
#endif


extern "C" {
	MuPdfApi::PdfDocument* m_doc;

	// The functions we will call from Unity.
	//
	EXPORT_API MuPdfApi::PdfDocument* Open(unsigned char* pdfdata, int bufferLength);

	EXPORT_API void ActivateDocument(MuPdfApi::PdfDocument* document);

	EXPORT_API int GetTextBytes(unsigned char* result);

	EXPORT_API unsigned char* GetBuffer();

	EXPORT_API int RenderPage(int width, int height);

	EXPORT_API int GetPageWidth();

	EXPORT_API int GetPageHeight();

	EXPORT_API int GetNumComponents();

	EXPORT_API int GetNumPages();

	EXPORT_API int GetCurrentPageNum();

	EXPORT_API bool GotoPage(int page);

	EXPORT_API bool GoToNextPage();

	EXPORT_API bool GoToPrevPage();

	EXPORT_API void Dispose(MuPdfApi::PdfDocument* document);

	EXPORT_API bool AddSelection(mu_point* pointList, int size);

	EXPORT_API int GetHighlights(fz_rect* rectList[], int max);
} // end of export C block
