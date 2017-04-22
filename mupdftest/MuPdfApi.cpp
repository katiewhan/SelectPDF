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

EXPORT_API int AddSelection(mu_point* pointList, int size) {
	return m_doc->AddSelection(pointList, size);
}

EXPORT_API int GetHighlights(int id, mu_rect* rectList[], int max) {
	/*mu_rect* r = new mu_rect[1];
	r[0].x0 = 1;
	r[0].x1 = 2;
	r[0].y0 = 3;
	r[0].y1 = 4;*/
	/*std::vector<fz_rect> recvec;
	recvec.push_back(r);
	*rectList = recvec.data();*/

	/**rectList = r;*/
	//(*rectList)[0] = 4;
	//(*rectList)[1] = 3;
	//(*rectList)[2] = 2;
	//(*rectList)[3] = 5;
	//(*rectList)[4] = 6;
	//return 1;
	return m_doc->GetHighlights(id, rectList, max);
}

EXPORT_API int GetNumSelections() {
	return m_doc->GetNumSelections();
}

EXPORT_API char* GetSelectionContent(int id) {
	return m_doc->GetSelectionContent(id);
}

EXPORT_API int GetSelectionContents(int id, mu_selection* contentList[], int max) {
	return m_doc->GetSelectionContents(id, contentList, max);
}