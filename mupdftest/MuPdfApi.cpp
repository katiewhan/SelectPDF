#include "PdfDocument.h"
#include "MuPdfApi.h"

//	MuPdfApi::PdfDocument* m_doc;
// The functions we will call from Unity.
//
EXPORT_API MuPdfApi::PdfDocument* Open(unsigned char* pdfdata, int bufferLength) {
	m_doc = new MuPdfApi::PdfDocument();
	m_doc->Open(pdfdata, bufferLength);
	return m_doc;
}

EXPORT_API void ActivateDocument(MuPdfApi::PdfDocument* document) {
	m_doc = document;
}

EXPORT_API int GetTextBytes(unsigned char* result) {
	return m_doc->GetTextBytes(result);
}

EXPORT_API unsigned char* GetBuffer() {
	return m_doc->GetBuffer();
}

EXPORT_API int RenderPage(int width, int height) {
	return m_doc->Render(width, height);
}

EXPORT_API int GetPageWidth() {
	return m_doc->GetPageWidth();
}

EXPORT_API int GetPageHeight() {
	return m_doc->GetPageHeight();
}

EXPORT_API int GetNumComponents() {
	return m_doc->GetNumComponents();
}

EXPORT_API int GetNumPages() {
	return m_doc->GetNumPages();
}

EXPORT_API int GetCurrentPageNum() {
	return m_doc->GetCurrentPageNum();
}

EXPORT_API bool GotoPage(int page) {
	return m_doc->GotoPage(page);
}

EXPORT_API bool GoToNextPage() {
	return m_doc->GoToNextPage();
}

EXPORT_API bool GoToPrevPage() {
	return m_doc->GoToPrevPage();
}

EXPORT_API void Dispose(MuPdfApi::PdfDocument* document) {
	document->Dispose();
	delete document;
}

EXPORT_API bool AddSelection(mu_point* pointList, int size) {
	return m_doc->AddSelection(pointList, size);
}

EXPORT_API int GetHighlights(fz_rect* rectList[], int max) {
	return m_doc->GetHighlights(rectList, max);
}